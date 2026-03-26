// Minimal msettings.h stub for Android
#ifndef MSETTINGS_H
#define MSETTINGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Stub implementations - Android uses SharedPreferences instead

#define SHARPNESS_SOFT 0
#define SHARPNESS_SHARP 1

#define EFFECT_NONE 0

// Simple stub functions
static inline int GetInt(const char* key, int defaultValue) { return defaultValue; }
static inline void SetInt(const char* key, int value) {}
static inline float GetFloat(const char* key, float defaultValue) { return defaultValue; }
static inline void SetFloat(const char* key, float value) {}
static inline int GetBool(const char* key, int defaultValue) { return defaultValue; }
static inline void SetBool(const char* key, int value) {}
static inline char* GetString(const char* key, const char* defaultValue) { return (char*)defaultValue; }
static inline void SetString(const char* key, const char* value) {}

#ifdef __cplusplus
}
#endif

#endif // MSETTINGS_H