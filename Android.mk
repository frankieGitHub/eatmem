LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES = eatmem.c

LOCAL_MODULE := eatmem

include $(BUILD_EXECUTABLE)
