# Android.mk for MinArch - With SDL2 dependency
MY_LOCAL_PATH := $(call my-dir)

# Include SDL2 build from its own Android.mk
include $(MY_LOCAL_PATH)/SDL2/Android.mk

# MinArch core - with SDL2
include $(CLEAR_VARS)
LOCAL_MODULE := minarch
LOCAL_C_INCLUDES := $(MY_LOCAL_PATH) \
                    $(MY_LOCAL_PATH)/minarch \
                    $(MY_LOCAL_PATH)/common \
                    $(MY_LOCAL_PATH)/platform \
                    $(MY_LOCAL_PATH)/SDL2/include

LOCAL_SRC_FILES := $(MY_LOCAL_PATH)/minarch/jni_entry.c \
                   $(MY_LOCAL_PATH)/minarch/minarch_android.c \
                   $(MY_LOCAL_PATH)/platform/platform_android.c

LOCAL_CFLAGS := -DPLATFORM=\"android\" \
                -DUSE_SDL2 \
                -DANDROID \
                -fomit-frame-pointer \
                -Ofast \
                -Os \
                -Wno-unused-function \
                -Wno-unused-variable

LOCAL_LDFLAGS := -ldl -lz -lm -llog
LOCAL_LDLIBS := -lEGL -landroid
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_STATIC_LIBRARIES := SDL2_main

include $(BUILD_SHARED_LIBRARY)