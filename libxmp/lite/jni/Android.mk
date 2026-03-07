LOCAL_PATH	:= $(call my-dir)/..

include $(CLEAR_VARS)

include $(LOCAL_PATH)/src/Makefile
include $(LOCAL_PATH)/src/loaders/Makefile

SRC_SOURCES	:= $(addprefix src/,$(SRC_OBJS))
LOADERS_SOURCES := $(addprefix src/loaders/,$(LOADERS_OBJS))

LOCAL_MODULE    := xmp
<<<<<<< HEAD
LOCAL_CFLAGS	:= -O3 -DHAVE_MKSTEMP -DHAVE_FNMATCH -DHAVE_POWF -DLIBXMP_CORE_PLAYER \
=======
LOCAL_CFLAGS	:= -O3 -DHAVE_MKSTEMP -DHAVE_FNMATCH -DHAVE_DIRENT -DHAVE_POWF \
		   -DLIBXMP_CORE_PLAYER \
>>>>>>> db7344ebf (abc)
		   -I$(LOCAL_PATH)/include/libxmp-lite
LOCAL_SRC_FILES := $(SRC_SOURCES:.o=.c.arm) \
		   $(LOADERS_SOURCES:.o=.c)

include $(BUILD_STATIC_LIBRARY)
