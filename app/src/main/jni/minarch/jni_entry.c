// JNI Entry Point for MinArch Android
// Simplified - no SDL dependency, uses native Android APIs

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "MinArch"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Forward declarations
extern int minarch_init(int width, int height);
extern void minarch_quit(void);
extern int minarch_load_core(const char* path);
extern int minarch_load_rom(const char* path);
extern int minarch_save_state(void);
extern int minarch_load_state(void);
extern void minarch_set_video_surface(void* surface);
extern void minarch_set_video_size(int width, int height);
extern void minarch_pause(void);
extern void minarch_resume(void);
extern void minarch_run_frame(void);
extern int minarch_handle_key(int keycode, int pressed);
extern int minarch_handle_joystick(float* axisValues, int numAxes);

// JNI_OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("JNI_OnLoad called");
    return JNI_VERSION_1_6;
}

// Initialize with surface
JNIEXPORT void JNICALL Java_com_minarch_MinArchNative_surfaceCreated(
    JNIEnv* env, jobject thiz, jobject surface) {

    LOGI("Surface created");

    if (!surface) {
        LOGE("Null surface");
        return;
    }

    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window) {
        int width = ANativeWindow_getWidth(window);
        int height = ANativeWindow_getHeight(window);
        LOGI("Window: %dx%d", width, height);

        minarch_set_video_surface(window);
        minarch_init(width, height);
    }
}

// Surface changed
JNIEXPORT void JNICALL Java_com_minarch_MinArchNative_surfaceChanged(
    JNIEnv* env, jobject thiz, jobject surface, jint width, jint height) {

    LOGI("Surface changed: %dx%d", width, height);
    minarch_set_video_size(width, height);
}

// Surface destroyed
JNIEXPORT void JNICALL Java_com_minarch_MinArchNative_surfaceDestroyed(
    JNIEnv* env, jobject thiz) {

    LOGI("Surface destroyed");
    // Don't destroy MinArch here, just detach surface
}

// Load ROM
JNIEXPORT jboolean JNICALL Java_com_minarch_MinArchNative_loadRom(
    JNIEnv* env, jobject thiz, jstring path) {

    const char* romPath = (*env)->GetStringUTFChars(env, path, NULL);
    LOGI("Loading ROM: %s", romPath);

    int result = minarch_load_rom(romPath);

    (*env)->ReleaseStringUTFChars(env, path, romPath);

    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

// Load Core
JNIEXPORT jboolean JNICALL Java_com_minarch_MinArchNative_loadCore(
    JNIEnv* env, jobject thiz, jstring path) {

    const char* corePath = (*env)->GetStringUTFChars(env, path, NULL);
    LOGI("Loading Core: %s", corePath);

    int result = minarch_load_core(corePath);

    (*env)->ReleaseStringUTFChars(env, path, corePath);

    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

// Save State
JNIEXPORT jboolean JNICALL Java_com_minarch_MinArchNative_saveState(
    JNIEnv* env, jobject thiz) {

    return minarch_save_state() == 0 ? JNI_TRUE : JNI_FALSE;
}

// Load State
JNIEXPORT jboolean JNICALL Java_com_minarch_MinArchNative_loadState(
    JNIEnv* env, jobject thiz) {

    return minarch_load_state() == 0 ? JNI_TRUE : JNI_FALSE;
}

// Resume
JNIEXPORT void JNICALL Java_com_minarch_MinArchNative_resume(
    JNIEnv* env, jobject thiz) {

    LOGI("Resume");
    minarch_resume();
}

// Pause
JNIEXPORT void JNICALL Java_com_minarch_MinArchNative_pause(
    JNIEnv* env, jobject thiz) {

    LOGI("Pause");
    minarch_pause();
}

// Destroy
JNIEXPORT void JNICALL Java_com_minarch_MinArchNative_destroy(
    JNIEnv* env, jobject thiz) {

    LOGI("Destroy");
    minarch_quit();
}

// Key Down
JNIEXPORT jboolean JNICALL Java_com_minarch_MinArchNative_onKeyDown(
    JNIEnv* env, jobject thiz, jint keyCode) {

    return minarch_handle_key(keyCode, 1) ? JNI_TRUE : JNI_FALSE;
}

// Key Up
JNIEXPORT jboolean JNICALL Java_com_minarch_MinArchNative_onKeyUp(
    JNIEnv* env, jobject thiz, jint keyCode) {

    return minarch_handle_key(keyCode, 0) ? JNI_TRUE : JNI_FALSE;
}

// Joystick
JNIEXPORT jboolean JNICALL Java_com_minarch_MinArchNative_onJoystickMoved(
    JNIEnv* env, jobject thiz, jfloatArray axisValues) {

    jfloat* axes = (*env)->GetFloatArrayElements(env, axisValues, NULL);
    jsize numAxes = (*env)->GetArrayLength(env, axisValues);

    int result = minarch_handle_joystick(axes, numAxes);

    (*env)->ReleaseFloatArrayElements(env, axisValues, axes, 0);

    return result ? JNI_TRUE : JNI_FALSE;
}

// Set paths from Java
JNIEXPORT void JNICALL Java_com_minarch_MinArchNative_setPaths(
    JNIEnv* env, jobject thiz,
    jstring internalPath, jstring savePath, jstring cachePath, jstring romsPath) {
    // Paths are handled in platform_android.c
}