LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := mRF_native
LOCAL_LDLIBS 	:= -llog
LOCAL_SRC_FILES := mRF.c
					
include $(BUILD_EXECUTABLE) 