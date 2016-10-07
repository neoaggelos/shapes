LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	../../../src/difficulty.cpp \
	../../../src/entername.cpp \
	../../../src/game.cpp \
	../../../src/highscores.cpp \
	../../../src/main.cpp \
	../../../src/render.cpp \
	../../../src/settings.cpp \
	../../../src/shape.cpp \
	../../../src/super.cpp \
	../../../src/textrenderer.cpp \
	../../../src/tutorial.cpp \
	../../../src/audiodata.cpp \
	../../../src/util.cpp

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer SDLU

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
