LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/adler32.c \
	$(LOCAL_PATH)/crc32.c \
	$(LOCAL_PATH)/infblock.c \
	$(LOCAL_PATH)/inffast.c \
	$(LOCAL_PATH)/inftrees.c \
	$(LOCAL_PATH)/uncompr.c \
	$(LOCAL_PATH)/zutil.c \
	$(LOCAL_PATH)/compress.c \
	$(LOCAL_PATH)/deflate.c \
	$(LOCAL_PATH)/gzio.c \
	$(LOCAL_PATH)/infcodes.c \
	$(LOCAL_PATH)/inflate.c \
	$(LOCAL_PATH)/infutil.c \
	$(LOCAL_PATH)/trees.c \
	$(LOCAL_PATH)/unzip.c \

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

#LOCAL_CFLAGS += -DUSE_FILE32API

#LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := zlib

#ifeq ($(TARGET_IS_64_BIT),true)
#LOCAL_CFLAGS += -DTARGET_IS_64_BIT
#LOCAL_CPPFLAGS += -DTARGET_IS_64_BIT

#LOCAL_MULTILIB := 64
#else
#LOCAL_MULTILIB := 32
#endif

#LOCAL_MULTILIB := both

#LOCAL_CFLAGS += -Wall

include $(BUILD_SHARED_LIBRARY)
