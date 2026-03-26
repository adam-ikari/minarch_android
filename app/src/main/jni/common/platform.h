// platform.h - Android version
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <time.h>

// Android-specific paths (will be overridden at runtime)
#define SDCARD_PATH "/storage/emulated/0"
#define PLATFORM "android"
#define FIXED_WIDTH 1920
#define FIXED_HEIGHT 1080
#define FIXED_PITCH (FIXED_WIDTH * 4)
#define FIXED_SIZE (FIXED_PITCH * FIXED_HEIGHT)
#define FIXED_SCALE 1
#define FIXED_BPP 4  // 32-bit color (4 bytes per pixel)

// Power button handling
#define BUTTON_POWER -1
#define CODE_POWER -1
#define JOY_POWER -1

// Menu button handling
#define BUTTON_MENU -1
#define CODE_MENU -1
#define JOY_MENU -1
#define BUTTON_MENU_ALT -1
#define CODE_MENU_ALT -1
#define JOY_MENU_ALT -1

// Input
#define MAX_JOYS 4
#define MAX_AXES 16
#define MAX_BUTTONS 32

// Input codes for Android
#define CODE_UP 19    // KEY_DPAD_UP
#define CODE_DOWN 20  // KEY_DPAD_DOWN
#define CODE_LEFT 21  // KEY_DPAD_LEFT
#define CODE_RIGHT 22 // KEY_DPAD_RIGHT
#define CODE_A 96     // BUTTON_A
#define CODE_B 97     // BUTTON_B
#define CODE_X 99     // BUTTON_X
#define CODE_Y 100    // BUTTON_Y
#define CODE_START 108  // BUTTON_START
#define CODE_SELECT 109 // BUTTON_SELECT
#define CODE_L1 102    // BUTTON_L1
#define CODE_R1 103    // BUTTON_R1
#define CODE_L2 104    // BUTTON_L2
#define CODE_R2 105    // BUTTON_R2
#define CODE_MENU 82   // BUTTON_MENU

// Axis
#define AXIS_LX 0
#define AXIS_LY 1
#define AXIS_RX 2
#define AXIS_RY 3

// LED
#define LED_NONE -1

// Platform functions
extern int platform_init(int width, int height);
extern void platform_cleanup(void);
extern const char* platform_get_model_name(void);
extern const char* platform_get_device_name(void);

// Display
extern void platform_set_stretch(int stretch);
extern int platform_get_physical_width(void);
extern int platform_get_physical_height(void);

// Power
extern void platform_sleep(void);
extern void platform_power_off(void);
extern void platform_shutdown(void);
extern void platform_reboot(void);

// Settings
extern int platform_get_brightness(void);
extern void platform_set_brightness(int brightness);
extern int platform_get_volume(void);
extern void platform_set_volume(int volume);

// Battery
extern int platform_get_battery_level(void);
extern int platform_is_charging(void);

// Input
extern int platform_get_input(void);

// LED
extern void platform_set_power_led(int on);

// OSD
extern void platform_set_osd_visibility(int visible);

// CPU
extern void platform_set_cpu_speed(int mhz);

// Accelerometer
extern int platform_has_accelerometer(void);
extern void platform_get_accelerometer(float* x, float* y, float* z);

// HDMI
extern int platform_has_hdmi_audio(void);
extern void platform_set_hdmi_audio(int enable);

// Time
extern time_t platform_get_rtc_time(void);
extern void platform_set_rtc_time(time_t t);

// MinUI
extern void platform_enable_minui(int enable);

// Audio
extern int platform_init_audio(int rate, int channels);
extern void platform_cleanup_audio(void);

// FFmpeg
extern void platform_set_ffmpeg(int enable);

#endif // __PLATFORM_H__