// Android platform header
#ifndef PLATFORM_ANDROID_H
#define PLATFORM_ANDROID_H

#include <time.h>
#include <jni.h>

// Display
void* platform_get_window(void);
void platform_set_window(void* window);

// Input
int platform_get_input(void);

// File paths
char* platform_get_config_path(void);
char* platform_get_save_path(void);
char* platform_get_temp_path(void);
const char* get_roms_path(void);

// Settings
int platform_get_brightness(void);
void platform_set_brightness(int brightness);
int platform_get_volume(void);
void platform_set_volume(int volume);

// Power
void platform_power_off(void);
void platform_sleep(void);
void platform_shutdown(void);
void platform_reboot(void);

// Time
time_t platform_get_rtc_time(void);
void platform_set_rtc_time(time_t t);

// Display modes
void platform_set_stretch(int stretch);

// Audio
int platform_init_audio(int rate, int channels);
void platform_cleanup_audio(void);

// LED
void platform_set_power_led(int on);

// OSD
void platform_set_osd_visibility(int visible);

// CPU
void platform_set_cpu_speed(int mhz);

// Model info
const char* platform_get_model_name(void);
const char* platform_get_device_name(void);

// Physical display
int platform_get_physical_width(void);
int platform_get_physical_height(void);

// Accelerometer
int platform_has_accelerometer(void);
void platform_get_accelerometer(float* x, float* y, float* z);

// Battery
int platform_get_battery_level(void);
int platform_is_charging(void);

// HDMI
int platform_has_hdmi_audio(void);
void platform_set_hdmi_audio(int enable);

// MinUI
void platform_enable_minui(int enable);

// FFmpeg
void platform_set_ffmpeg(int enable);

// Initialization
int platform_init(int width, int height);
void platform_cleanup(void);

// JNI helpers
JNIEnv* get_jni_env(void);
JavaVM* get_java_vm(void);
void set_java_vm(JavaVM* vm);

#endif // PLATFORM_ANDROID_H