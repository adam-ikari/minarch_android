// MinArch Android - Simplified implementation using native Android APIs
// No SDL2 dependency - uses Android Native Window and EGL directly

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <errno.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

#include "libretro.h"
#include "../common/defines.h"
#include "../common/utils.h"
#include "../platform/platform_android.h"

#define LOG_TAG "MinArch"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

// Global state
static int g_running = 0;
static int g_paused = 0;
static void* g_core_handle = NULL;
static char g_core_path[MAX_PATH];
static char g_rom_path[MAX_PATH];

// Display
static EGLDisplay g_eglDisplay = EGL_NO_DISPLAY;
static EGLSurface g_eglSurface = EGL_NO_SURFACE;
static EGLContext g_eglContext = EGL_NO_CONTEXT;
static ANativeWindow* g_window = NULL;
static int g_window_width = 0;
static int g_window_height = 0;

// Video buffer
static uint32_t* g_video_buffer = NULL;
static int g_video_width = 0;
static int g_video_height = 0;

// Input state
static int16_t g_input_state[4][RETRO_DEVICE_ID_JOYPAD_R3 + 1];

// Core function pointers
static retro_set_environment_t retro_set_environment;
static retro_set_video_refresh_t retro_set_video_refresh;
static retro_set_audio_sample_t retro_set_audio_sample;
static retro_set_audio_sample_batch_t retro_set_audio_sample_batch;
static retro_set_input_poll_t retro_set_input_poll;
static retro_set_input_state_t retro_set_input_state;
static retro_api_version_t retro_api_version;
static retro_get_system_info_t retro_get_system_info;
static retro_get_system_av_info_t retro_get_system_av_info;
static retro_init_t retro_init;
static retro_deinit_t retro_deinit;
static retro_load_game_t retro_load_game;
static retro_run_t retro_run;
static retro_reset_t retro_reset;
static retro_unload_game_t retro_unload_game;
static retro_get_region_t retro_get_region;
static retro_serialize_t retro_serialize;
static retro_unserialize_t retro_unserialize;

// Forward declarations
static void video_callback(const void* data, unsigned width, unsigned height, size_t pitch);
static void audio_sample_callback(int16_t left, int16_t right);
static void audio_batch_callback(const int16_t* data, size_t count);
static void input_poll_callback(void);
static int16_t input_state_callback(unsigned port, unsigned device, unsigned index, unsigned id);
static bool environment_callback(unsigned cmd, void* data);

// EGL initialization
static int init_egl(void) {
    LOGI("Initializing EGL...");

    // Get EGL display
    g_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_eglDisplay == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return -1;
    }

    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(g_eglDisplay, &major, &minor)) {
        LOGE("Failed to initialize EGL");
        return -1;
    }
    LOGI("EGL version: %d.%d", major, minor);

    // Choose config
    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(g_eglDisplay, configAttribs, &config, 1, &numConfigs) || numConfigs == 0) {
        LOGE("Failed to choose EGL config");
        return -1;
    }

    // Create context
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    g_eglContext = eglCreateContext(g_eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
    if (g_eglContext == EGL_NO_CONTEXT) {
        LOGE("Failed to create EGL context");
        return -1;
    }

    // Create window surface
    if (g_window) {
        ANativeWindow_setBuffersGeometry(g_window, 0, 0, WINDOW_FORMAT_RGBX_8888);
        g_eglSurface = eglCreateWindowSurface(g_eglDisplay, config, g_window, NULL);
        if (g_eglSurface == EGL_NO_SURFACE) {
            LOGE("Failed to create EGL surface");
            return -1;
        }
    }

    // Make context current
    if (!eglMakeCurrent(g_eglDisplay, g_eglSurface, g_eglSurface, g_eglContext)) {
        LOGE("Failed to make EGL context current");
        return -1;
    }

    LOGI("EGL initialized successfully");
    return 0;
}

static void cleanup_egl(void) {
    LOGI("Cleaning up EGL...");

    if (g_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (g_eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(g_eglDisplay, g_eglSurface);
            g_eglSurface = EGL_NO_SURFACE;
        }

        if (g_eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(g_eglDisplay, g_eglContext);
            g_eglContext = EGL_NO_CONTEXT;
        }

        eglTerminate(g_eglDisplay);
        g_eglDisplay = EGL_NO_DISPLAY;
    }
}

// Initialize the emulator
int minarch_init(int width, int height) {
    LOGI("MinArch init: %dx%d", width, height);

    // Initialize platform
    platform_init(width, height);

    // Clear input state
    memset(g_input_state, 0, sizeof(g_input_state));

    // Initialize EGL
    if (init_egl() != 0) {
        LOGW("EGL init failed, continuing without GL");
    }

    g_running = 1;
    return 0;
}

// Set video surface from JNI
void minarch_set_video_surface(void* surface) {
    g_window = (ANativeWindow*)surface;

    if (g_window) {
        g_window_width = ANativeWindow_getWidth(g_window);
        g_window_height = ANativeWindow_getHeight(g_window);
        LOGI("Surface set: %dx%d", g_window_width, g_window_height);

        // Re-initialize EGL if needed
        if (g_eglDisplay == EGL_NO_DISPLAY) {
            init_egl();
        }
    }
}

void minarch_set_video_size(int width, int height) {
    g_window_width = width;
    g_window_height = height;
}

// Load a libretro core
int minarch_load_core(const char* path) {
    LOGI("Loading core: %s", path);

    if (g_core_handle) {
        dlclose(g_core_handle);
        g_core_handle = NULL;
    }

    g_core_handle = dlopen(path, RTLD_LAZY);
    if (!g_core_handle) {
        LOGE("Failed to load core: %s", dlerror());
        return -1;
    }

    // Get core functions
    retro_init = (retro_init_t)dlsym(g_core_handle, "retro_init");
    retro_deinit = (retro_deinit_t)dlsym(g_core_handle, "retro_deinit");
    retro_api_version = (retro_api_version_t)dlsym(g_core_handle, "retro_api_version");
    retro_get_system_info = (retro_get_system_info_t)dlsym(g_core_handle, "retro_get_system_info");
    retro_get_system_av_info = (retro_get_system_av_info_t)dlsym(g_core_handle, "retro_get_system_av_info");
    retro_load_game = (retro_load_game_t)dlsym(g_core_handle, "retro_load_game");
    retro_run = (retro_run_t)dlsym(g_core_handle, "retro_run");
    retro_reset = (retro_reset_t)dlsym(g_core_handle, "retro_reset");
    retro_unload_game = (retro_unload_game_t)dlsym(g_core_handle, "retro_unload_game");
    retro_get_region = (retro_get_region_t)dlsym(g_core_handle, "retro_get_region");
    retro_serialize = (retro_serialize_t)dlsym(g_core_handle, "retro_serialize");
    retro_unserialize = (retro_unserialize_t)dlsym(g_core_handle, "retro_unserialize");

    if (!retro_init || !retro_run) {
        LOGE("Core missing required functions");
        dlclose(g_core_handle);
        g_core_handle = NULL;
        return -1;
    }

    strncpy(g_core_path, path, sizeof(g_core_path) - 1);
    LOGI("Core loaded successfully");
    return 0;
}

// Load a ROM
int minarch_load_rom(const char* path) {
    LOGI("Loading ROM: %s", path);

    if (!g_core_handle) {
        LOGE("No core loaded");
        return -1;
    }

    strncpy(g_rom_path, path, sizeof(g_rom_path) - 1);

    // Set up callbacks
    if (retro_set_environment) retro_set_environment(environment_callback);
    if (retro_set_video_refresh) retro_set_video_refresh(video_callback);
    if (retro_set_audio_sample) retro_set_audio_sample(audio_sample_callback);
    if (retro_set_audio_sample_batch) retro_set_audio_sample_batch(audio_batch_callback);
    if (retro_set_input_poll) retro_set_input_poll(input_poll_callback);
    if (retro_set_input_state) retro_set_input_state(input_state_callback);

    // Initialize core
    if (retro_init) retro_init();

    // Get system info
    if (retro_get_system_info) {
        struct retro_system_info info;
        retro_get_system_info(&info);
        LOGI("Core: %s %s", info.library_name ? info.library_version : "unknown",
             info.library_version ? info.library_version : "unknown");
    }

    // Load game
    struct retro_game_info game_info;
    game_info.path = path;
    game_info.data = NULL;
    game_info.size = 0;

    // Read file if needed
    FILE* f = fopen(path, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        game_info.size = ftell(f);
        fseek(f, 0, SEEK_SET);
        game_info.data = malloc(game_info.size);
        fread((void*)game_info.data, 1, game_info.size, f);
        fclose(f);
    }

    if (retro_load_game && !retro_load_game(&game_info)) {
        LOGE("Failed to load game");
        if (game_info.data) free((void*)game_info.data);
        return -1;
    }

    if (game_info.data) free((void*)game_info.data);

    // Get AV info
    if (retro_get_system_av_info) {
        struct retro_system_av_info av_info;
        retro_get_system_av_info(&av_info);

        g_video_width = av_info.geometry.base_width;
        g_video_height = av_info.geometry.base_height;
        LOGI("Video: %dx%d, FPS: %.2f", g_video_width, g_video_height, av_info.timing.fps);

        // Allocate video buffer
        if (g_video_buffer) free(g_video_buffer);
        g_video_buffer = (uint32_t*)malloc(g_video_width * g_video_height * 4);
    }

    LOGI("ROM loaded successfully");
    return 0;
}

// Run one frame
void minarch_run_frame(void) {
    if (!g_running || g_paused || !retro_run) return;
    retro_run();
}

// Save state
int minarch_save_state(void) {
    if (!g_core_handle || !retro_serialize) return -1;

    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/save_state.sav", platform_get_save_path());

    unsigned char buffer[1024*1024];
    size_t size = retro_serialize(buffer, sizeof(buffer));
    if (size == 0) return -1;

    FILE* f = fopen(path, "wb");
    if (!f) return -1;

    fwrite(buffer, 1, size, f);
    fclose(f);

    LOGI("State saved: %s (%zu bytes)", path, size);
    return 0;
}

// Load state
int minarch_load_state(void) {
    if (!g_core_handle || !retro_unserialize) return -1;

    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/save_state.sav", platform_get_save_path());

    FILE* f = fopen(path, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* buffer = malloc(size);
    fread(buffer, 1, size, f);
    fclose(f);

    int result = (retro_unserialize(buffer, size) == true) ? 0 : -1;
    free(buffer);

    LOGI("State loaded: %s", path);
    return result;
}

// Quit
void minarch_quit(void) {
    LOGI("MinArch quit");

    g_running = 0;

    if (g_core_handle) {
        if (retro_unload_game) retro_unload_game();
        if (retro_deinit) retro_deinit();
        dlclose(g_core_handle);
        g_core_handle = NULL;
    }

    if (g_video_buffer) {
        free(g_video_buffer);
        g_video_buffer = NULL;
    }

    cleanup_egl();
    platform_cleanup();
}

// Pause/Resume
void minarch_pause(void) {
    g_paused = 1;
    LOGI("MinArch paused");
}

void minarch_resume(void) {
    g_paused = 0;
    LOGI("MinArch resumed");
}

// Input handling
void minarch_set_input(int port, int button, int pressed) {
    if (port >= 0 && port < 4 && button >= 0 && button <= RETRO_DEVICE_ID_JOYPAD_R3) {
        g_input_state[port][button] = pressed ? 1 : 0;
    }
}

int minarch_get_input(int port, int button) {
    if (port >= 0 && port < 4 && button >= 0 && button <= RETRO_DEVICE_ID_JOYPAD_R3) {
        return g_input_state[port][button];
    }
    return 0;
}

// Video callback - render to native window
static void video_callback(const void* data, unsigned width, unsigned height, size_t pitch) {
    if (!data || !g_window) return;

    // Copy video data to buffer
    if (g_video_buffer && (g_video_width != width || g_video_height != height)) {
        free(g_video_buffer);
        g_video_buffer = NULL;
    }

    if (!g_video_buffer) {
        g_video_buffer = (uint32_t*)malloc(width * height * 4);
        g_video_width = width;
        g_video_height = height;
    }

    // Convert from core format to ARGB8888
    // This is a simplified conversion - actual implementation depends on pixel format
    memcpy(g_video_buffer, data, height * pitch);

    // Lock window buffer
    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(g_window, &buffer, NULL) != 0) {
        return;
    }

    // Scale and copy to window
    uint32_t* dest = (uint32_t*)buffer.bits;
    int dest_stride = buffer.stride;

    for (int y = 0; y < height && y < buffer.height; y++) {
        memcpy(dest + y * dest_stride,
               (uint8_t*)g_video_buffer + y * pitch,
               width * 4);
    }

    ANativeWindow_unlockAndPost(g_window);
}

// Audio callbacks
static void audio_sample_callback(int16_t left, int16_t right) {
    // Audio output - could be queued for playback
}

static void audio_batch_callback(const int16_t* data, size_t count) {
    (void)data;
    (void)count;
}

// Input callbacks
static void input_poll_callback(void) {
    // Poll hardware input - handled by JNI
}

static int16_t input_state_callback(unsigned port, unsigned device, unsigned index, unsigned id) {
    if (device == RETRO_DEVICE_JOYPAD && port < 4 && id <= RETRO_DEVICE_ID_JOYPAD_R3) {
        return g_input_state[port][id];
    }
    return 0;
}

// Environment callback
static bool environment_callback(unsigned cmd, void* data) {
    switch (cmd) {
        case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: {
            return false;
        }
        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: {
            *(const char**)data = platform_get_config_path();
            return true;
        }
        case RETRO_ENVIRONMENT_GET_SAVESTATE_DIRECTORY: {
            *(const char**)data = platform_get_save_path();
            return true;
        }
        case RETRO_ENVIRONMENT_GET_CAN_DUPE: {
            *(bool*)data = true;
            return true;
        }
        case RETRO_ENVIRONMENT_GET_REGION: {
            *(unsigned*)data = RETRO_REGION_NTSC;
            return true;
        }
        default:
            return false;
    }
}

// Key handling from JNI
int minarch_handle_key(int keycode, int pressed) {
    // Map Android key codes to retro input
    int button = -1;

    switch (keycode) {
        case 19: button = RETRO_DEVICE_ID_JOYPAD_UP; break;      // DPAD_UP
        case 20: button = RETRO_DEVICE_ID_JOYPAD_DOWN; break;    // DPAD_DOWN
        case 21: button = RETRO_DEVICE_ID_JOYPAD_LEFT; break;    // DPAD_LEFT
        case 22: button = RETRO_DEVICE_ID_JOYPAD_RIGHT; break;   // DPAD_RIGHT
        case 96: button = RETRO_DEVICE_ID_JOYPAD_A; break;       // BUTTON_A
        case 97: button = RETRO_DEVICE_ID_JOYPAD_B; break;       // BUTTON_B
        case 99:  button = RETRO_DEVICE_ID_JOYPAD_X; break;      // BUTTON_X
        case 100: button = RETRO_DEVICE_ID_JOYPAD_Y; break;      // BUTTON_Y
        case 108: button = RETRO_DEVICE_ID_JOYPAD_START; break;  // BUTTON_START
        case 109: button = RETRO_DEVICE_ID_JOYPAD_SELECT; break; // BUTTON_SELECT
        case 102: button = RETRO_DEVICE_ID_JOYPAD_L; break;      // BUTTON_L1
        case 103: button = RETRO_DEVICE_ID_JOYPAD_R; break;      // BUTTON_R1
        case 104: button = RETRO_DEVICE_ID_JOYPAD_L2; break;     // BUTTON_L2
        case 105: button = RETRO_DEVICE_ID_JOYPAD_R2; break;     // BUTTON_R2
    }

    if (button >= 0) {
        g_input_state[0][button] = pressed ? 1 : 0;
        return 1;
    }

    return 0;
}

int minarch_handle_joystick(float* axisValues, int numAxes) {
    // Handle joystick input
    // Map first 4 axes to D-pad for simple gamepad
    if (numAxes >= 2) {
        g_input_state[0][RETRO_DEVICE_ID_JOYPAD_LEFT]  = (axisValues[0] < -0.5f) ? 1 : 0;
        g_input_state[0][RETRO_DEVICE_ID_JOYPAD_RIGHT] = (axisValues[0] > 0.5f) ? 1 : 0;
        g_input_state[0][RETRO_DEVICE_ID_JOYPAD_UP]    = (axisValues[1] < -0.5f) ? 1 : 0;
        g_input_state[0][RETRO_DEVICE_ID_JOYPAD_DOWN]  = (axisValues[1] > 0.5f) ? 1 : 0;
    }
    return 0;
}