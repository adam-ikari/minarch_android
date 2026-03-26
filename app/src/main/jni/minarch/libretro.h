// Simplified libretro.h header for MinArch Android
// Based on libretro API

#ifndef LIBRETRO_H
#define LIBRETRO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Core API version
#define RETRO_API_VERSION 1

// Environment callbacks
enum retro_environment_cmd {
    RETRO_ENVIRONMENT_SET_ROTATION = 1,
    RETRO_ENVIRONMENT_GET_OVERSCAN = 2,
    RETRO_ENVIRONMENT_GET_CAN_DUPE = 3,
    RETRO_ENVIRONMENT_GET_VARIABLE = 4,
    RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE = 5,
    RETRO_ENVIRONMENT_SET_MESSAGE = 6,
    RETRO_ENVIRONMENT_SHOW_PRESS = 7,
    RETRO_ENVIRONMENT_GET_LOG_INTERFACE = 8,
    RETRO_ENVIRONMENT_GET_PERF_INTERFACE = 9,
    RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE = 10,
    RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY = 13,
    RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY = 11,
    RETRO_ENVIRONMENT_GET_SAVESTATE_DIRECTORY = 12,
    RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY = 14,
    RETRO_ENVIRONMENT_GET_VIDEO_OUTPUT_SIZE = 16,
    RETRO_ENVIRONMENT_GET_VIDEO_OUTPUT = 17,
    RETRO_ENVIRONMENT_GET_AUDIO_BUFFER_BASE = 19,
    RETRO_ENVIRONMENT_SET_HW_RENDER = 21,
    RETRO_ENVIRONMENT_GET_PREFERRED_HW_RENDER = 22,
    RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK = 24,
    RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE = 25,
    RETRO_ENVIRONMENT_GET_MIDI_INTERFACE = 26,
    RETRO_ENVIRONMENT_GET_REGION = 28,
    RETRO_ENVIRONMENT_GET_FOCUS = 30,
    RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS = 31,
};

// Video rotation
enum retro_rotation {
    RETRO_ROTATION_0 = 0,
    RETRO_ROTATION_90 = 1,
    RETRO_ROTATION_180 = 2,
    RETRO_ROTATION_270 = 3,
};

// Pixel formats
enum retro_pixel_format {
    RETRO_PIXEL_FORMAT_0RGB155 = 0,
    RETRO_PIXEL_FORMAT_XRGB8888 = 1,
    RETRO_PIXEL_FORMAT_RGB565 = 2,
};

// Region
enum retro_region {
    RETRO_REGION_NTSC = 0,
    RETRO_REGION_PAL = 1,
};

// Memory map
struct retro_memory_map {
    const struct retro_memory_descriptor *descriptors;
    unsigned num_descriptors;
};

struct retro_memory_descriptor {
    uint64_t flags;
    void *addr;
    size_t len;
    const char *alias;
    unsigned priority;
    unsigned start;
};

// Performance counters
struct retro_perf_counter {
    const char *ident;
    unsigned id;
    uint64_t start;
    uint64_t total;
    unsigned call_count;
};

// System info
struct retro_system_info {
    const char *library_name;
    const char *library_version;
    const char *valid_extensions;
    bool needs_fullpath;
    bool block_extract;
};

// Game geometry
struct retro_game_geometry {
    unsigned base_width;
    unsigned base_height;
    unsigned max_width;
    unsigned max_height;
    float aspect_ratio;
};

// System timing
struct retro_system_timing {
    double fps;
    double sample_rate;
};

// Audio video info
struct retro_audio_video_info {
    bool valid;
    unsigned int width;
    unsigned int height;
    float aspect_ratio;
    bool update;
    unsigned int frames;
};

// Log callback
typedef void (*retro_log_printf_t)(enum retro_log_level level, const char *fmt, ...);

enum retro_log_level {
    RETRO_LOG_DEBUG = 0,
    RETRO_LOG_INFO = 1,
    RETRO_LOG_WARN = 2,
    RETRO_LOG_ERROR = 3,
};

struct retro_log_callback {
    retro_log_printf_t log;
};

// Variable
struct retro_variable {
    const char *key;
    const char *value;
};

// Game info
struct retro_game_info {
    const void *data;
    size_t size;
    const char *path;
};

// Core callbacks
typedef void (*retro_video_refresh_t)(const void *data, unsigned width, unsigned height, size_t pitch);
typedef void (*retro_audio_sample_t)(int16_t left, int16_t right);
typedef void (*retro_audio_sample_batch_t)(const int16_t *data, size_t count);
typedef int16_t (*retro_input_state_t)(unsigned port, unsigned device, unsigned index, unsigned id);
typedef bool (*retro_load_game_t)(const struct retro_game_info *game);
typedef bool (*retro_load_game_special_t)(unsigned game_type, const struct retro_game_info *info, size_t num_info);
typedef void (*retro_unload_game_t)(void);
typedef unsigned (*retro_get_region_t)(void);
typedef void *(*retro_get_memory_data_t)(unsigned id);
typedef size_t (*retro_get_memory_size_t)(unsigned id);

// Core init/deinit
typedef void (*retro_init_t)(void);

// Core functions
typedef void (*retro_set_video_refresh_t)(retro_video_refresh_t cb);
typedef void (*retro_set_audio_sample_t)(retro_audio_sample_t cb);
typedef void (*retro_set_audio_sample_batch_t)(retro_audio_sample_batch_t cb);
typedef void (*retro_set_input_poll_t)(void (*cb)(void));
typedef void (*retro_set_input_state_t)(int16_t (*cb)(unsigned port, unsigned device, unsigned index, unsigned id));
typedef void (*retro_set_environment_t)(bool (*cb)(unsigned cmd, void *data));
typedef void (*retro_deinit_t)(void);
typedef unsigned (*retro_api_version_t)(void);
typedef void (*retro_set_controller_port_device_t)(unsigned port, unsigned device);
typedef void (*retro_reset_t)(void);
typedef void (*retro_run_t)(void);
typedef size_t (*retro_serialize_t)(void *data, size_t size);
typedef bool (*retro_unserialize_t)(const void *data, size_t size);
typedef void (*retro_cheat_reset_t)(void);
typedef void (*retro_cheat_set_t)(unsigned index, bool enabled, const char *code);
typedef bool (*retro_get_system_info_t)(struct retro_system_info *info);
typedef bool (*retro_get_system_av_info_t)(struct retro_system_av_info *info);

// System AV info
struct retro_system_av_info {
    struct retro_game_geometry geometry;
    struct retro_system_timing timing;
};

// Memory areas
enum retro_memory_type {
    RETRO_MEMORY_MASK = 0xff,
    RETRO_MEMORY_SYSTEM_RAM = 0,
    RETRO_MEMORY_SAVE_RAM = 1,
    RETRO_MEMORY_VIDEO_RAM = 2,
    RETRO_MEMORY_SNES_GAME_BOY_APU_RAM = 5,
    RETRO_MEMORY_SNES_GAME_BOY_CPU_RAM = 6,
    RETRO_MEMORY_SNES_GAME_BOY2_CPU_RAM = 7,
    RETRO_MEMORY_SNES_SUBSYSTEM_CPU_RAM = 8,
};

// Input descriptors
enum retro_device {
    RETRO_DEVICE_JOYPAD = 0,
    RETRO_DEVICE_MOUSE = 1,
    RETRO_DEVICE_KEYBOARD = 2,
    RETRO_DEVICE_LIGHTGUN = 3,
    RETRO_DEVICE_ANALOG = 4,
    RETRO_DEVICE_POINTER = 5,
};

enum retro_device_id_joypad {
    RETRO_DEVICE_ID_JOYPAD_B = 0,
    RETRO_DEVICE_ID_JOYPAD_Y = 1,
    RETRO_DEVICE_ID_JOYPAD_SELECT = 2,
    RETRO_DEVICE_ID_JOYPAD_START = 3,
    RETRO_DEVICE_ID_JOYPAD_UP = 4,
    RETRO_DEVICE_ID_JOYPAD_DOWN = 5,
    RETRO_DEVICE_ID_JOYPAD_LEFT = 6,
    RETRO_DEVICE_ID_JOYPAD_RIGHT = 7,
    RETRO_DEVICE_ID_JOYPAD_A = 8,
    RETRO_DEVICE_ID_JOYPAD_X = 9,
    RETRO_DEVICE_ID_JOYPAD_L = 10,
    RETRO_DEVICE_ID_JOYPAD_R = 11,
    RETRO_DEVICE_ID_JOYPAD_L2 = 12,
    RETRO_DEVICE_ID_JOYPAD_R2 = 13,
    RETRO_DEVICE_ID_JOYPAD_L3 = 14,
    RETRO_DEVICE_ID_JOYPAD_R3 = 15,
};

// Linkage
#ifdef __cplusplus
}
#endif

#endif // LIBRETRO_H