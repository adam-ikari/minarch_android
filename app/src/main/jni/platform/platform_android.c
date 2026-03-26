// Android platform implementation for MinArch
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/native_window.h>
#include <jni.h>

#include "../common/platform.h"

#define LOG_TAG "MinArch"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Global references
static JavaVM* g_javaVM = NULL;
static jobject g_surface = NULL;
static ANativeWindow* g_window = NULL;
static int g_window_width = 0;
static int g_window_height = 0;

// Override default paths for Android
static char g_internal_path[256] = "/data/data/com.minarch/files";
static char g_save_path[256] = "/data/data/com.minarch/files/saves";
static char g_cache_path[256] = "/data/data/com.minarch/cache";
static char g_roms_path[256] = "/storage/emulated/0/Roms";

// Platform state
static int g_initialized = 0;
static int g_brightness = 100;
static int g_volume = 100;
static int g_stretch = 0;

void JNICALL Java_com_minarch_MinArchNative_setNativePaths(
    JNIEnv* env, jobject thiz,
    jstring internalPath, jstring savePath, jstring cachePath, jstring romsPath) {

    const char* ip = (*env)->GetStringUTFChars(env, internalPath, NULL);
    const char* sp = (*env)->GetStringUTFChars(env, savePath, NULL);
    const char* cp = (*env)->GetStringUTFChars(env, cachePath, NULL);
    const char* rp = (*env)->GetStringUTFChars(env, romsPath, NULL);

    strncpy(g_internal_path, ip, sizeof(g_internal_path) - 1);
    strncpy(g_save_path, sp, sizeof(g_save_path) - 1);
    strncpy(g_cache_path, cp, sizeof(g_cache_path) - 1);
    strncpy(g_roms_path, rp, sizeof(g_roms_path) - 1);

    // Create directories
    mkdir(g_internal_path, 0755);
    mkdir(g_save_path, 0755);
    mkdir(g_cache_path, 0755);

    (*env)->ReleaseStringUTFChars(env, internalPath, ip);
    (*env)->ReleaseStringUTFChars(env, savePath, sp);
    (*env)->ReleaseStringUTFChars(env, cachePath, cp);
    (*env)->ReleaseStringUTFChars(env, romsPath, rp);

    LOGI("Paths set: internal=%s, saves=%s, cache=%s, roms=%s",
         g_internal_path, g_save_path, g_cache_path, g_roms_path);
}

// Platform initialization
int platform_init(int width, int height) {
    if (g_initialized) return 0;

    LOGI("Platform init: %dx%d", width, height);

    g_window_width = width;
    g_window_height = height;

    g_initialized = 1;
    return 0;
}

void platform_cleanup(void) {
    LOGI("Platform cleanup");
    g_initialized = 0;
    g_window = NULL;
}

// Display
void* platform_get_window(void) {
    return g_window;
}

void platform_set_window(void* window) {
    g_window = (ANativeWindow*)window;
    if (g_window) {
        g_window_width = ANativeWindow_getWidth(g_window);
        g_window_height = ANativeWindow_getHeight(g_window);
        LOGI("Window set: %dx%d", g_window_width, g_window_height);
    }
}

int platform_get_physical_width(void) {
    return g_window_width;
}

int platform_get_physical_height(void) {
    return g_window_height;
}

void platform_set_stretch(int stretch) {
    g_stretch = stretch;
    LOGI("Stretch mode: %d", stretch);
}

// Input - handled via JNI callbacks
int platform_get_input(void) {
    return 0;
}

// File operations - use Android paths
char* platform_get_config_path(void) {
    return g_internal_path;
}

char* platform_get_save_path(void) {
    return g_save_path;
}

char* platform_get_temp_path(void) {
    return g_cache_path;
}

const char* get_roms_path(void) {
    return g_roms_path;
}

// Settings
int platform_get_brightness(void) {
    return g_brightness;
}

void platform_set_brightness(int brightness) {
    g_brightness = brightness;
    // Would need system permission to actually set brightness
}

int platform_get_volume(void) {
    return g_volume;
}

void platform_set_volume(int volume) {
    g_volume = volume;
    // Volume handled by Android system
}

// Power
void platform_power_off(void) {
    // Not implemented on Android - apps cannot power off device
}

void platform_sleep(void) {
    // Android doesn't support traditional sleep, use Activity lifecycle
}

void platform_shutdown(void) {
    platform_power_off();
}

void platform_reboot(void) {
    // Not implemented - requires root
}

// Time
time_t platform_get_rtc_time(void) {
    time_t t;
    time(&t);
    return t;
}

void platform_set_rtc_time(time_t t) {
    // Not implemented on Android
}

// LED
void platform_set_power_led(int on) {
    // No LED on most Android devices
}

// OSD
void platform_set_osd_visibility(int visible) {
    // Handled by Android window system
}

// CPU
void platform_set_cpu_speed(int mhz) {
    LOGI("CPU speed request: %d MHz", mhz);
    // Android doesn't allow direct CPU frequency control from apps
}

// Model info
const char* platform_get_model_name(void) {
    static char model[64] = "Android";
    return model;
}

const char* platform_get_device_name(void) {
    static char device[64] = "Android Device";
    return device;
}

// Accelerometer
int platform_has_accelerometer(void) {
    return 0; // TODO: Check via JNI
}

void platform_get_accelerometer(float* x, float* y, float* z) {
    *x = *y = *z = 0;
}

// Battery
int platform_get_battery_level(void) {
    return 100; // TODO: Get from Android API
}

int platform_is_charging(void) {
    return 1; // Assume charging for simplicity
}

// HDMI
int platform_has_hdmi_audio(void) {
    return 1;
}

void platform_set_hdmi_audio(int enable) {
    // Handled by Android
}

// MinUI
void platform_enable_minui(int enable) {
    LOGI("MinUI mode: %d", enable);
}

// Audio
int platform_init_audio(int rate, int channels) {
    LOGI("Audio init: %d Hz, %d channels", rate, channels);
    return 0;
}

void platform_cleanup_audio(void) {
    LOGI("Audio cleanup");
}

// FFmpeg
void platform_set_ffmpeg(int enable) {
    LOGI("FFmpeg: %d", enable);
}

// Get JNI environment
JNIEnv* get_jni_env(void) {
    JNIEnv* env;
    if ((*g_javaVM)->GetEnv(g_javaVM, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return NULL;
    }
    return env;
}

// Attach thread to JVM
JavaVM* get_java_vm(void) {
    return g_javaVM;
}

void set_java_vm(JavaVM* vm) {
    g_javaVM = vm;
}