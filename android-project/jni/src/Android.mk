LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	$(LOCAL_PATH)/../../../src/difficulty.cpp \
	$(LOCAL_PATH)/../../../src/entername.cpp \
	$(LOCAL_PATH)/../../../src/game.cpp \
	$(LOCAL_PATH)/../../../src/highscores.cpp \
	$(LOCAL_PATH)/../../../src/main.cpp \
	$(LOCAL_PATH)/../../../src/render.cpp \
	$(LOCAL_PATH)/../../../src/settings.cpp \
	$(LOCAL_PATH)/../../../src/shape.cpp \
	$(LOCAL_PATH)/../../../src/super.cpp \
	$(LOCAL_PATH)/../../../src/audiodata.cpp \
	$(LOCAL_PATH)/../../../src/util.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer SDLU

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
