# Android.mk for MinArch
MY_LOCAL_PATH := $(call my-dir)

# MinArch core
include $(CLEAR_VARS)
LOCAL_MODULE := minarch
LOCAL_C_INCLUDES := $(MY_LOCAL_PATH) \
                    $(MY_LOCAL_PATH)/minarch \
                    $(MY_LOCAL_PATH)/common \
                    $(MY_LOCAL_PATH)/platform

LOCAL_SRC_FILES := $(MY_LOCAL_PATH)/minarch/jni_entry.c \
                   $(MY_LOCAL_PATH)/minarch/minarch_android.c \
                   $(MY_LOCAL_PATH)/platform/platform_android.c

LOCAL_CFLAGS := -DPLATFORM=\"android\" \
                -DANDROID \
                -fomit-frame-pointer \
                -Ofast \
                -Os \
                -Wno-unused-function \
                -Wno-unused-variable

LOCAL_LDFLAGS := -ldl -lz -lm -llog
LOCAL_LDLIBS := -lEGL -landroid

include $(BUILD_SHARED_LIBRARY)