##
## app/Chicony/CEC_Chime_Control/make.inc
##
## 
## Copyright (C) 2012-2016, Ambarella, Inc.
##

ifeq ($(BUILD_AMBAREALLA_CHICONY_CHIME_CONTROLLER), y)

LOCAL_PATH   := $(call my-dir)
include $(CLEAR_VARS)
include $(BUILD_CHICONY)

AIR_DIR = $(AMB_TOPDIR)/app/

# Original location that using this param has been replace with "CHICONY_OUT_BIN_DIR", easy migration solution.
AIR_APP_BIN_DIR	= $(FAKEROOT_DIR)/usr/bin

LOCAL_TARGET := cec_mp4_audio_parser
LOCAL_SRCS := $(addprefix $(LOCAL_PATH)/, main.cpp mp4.cpp stream.cpp shm_usage.cpp)
LOCAL_LIBS := 
LOCAL_LIBS += 
LOCAL_CFLAGS :=
		

LOCAL_CFLAGS +=


LOCAL_LDFLAGS:= -lpthread 
LOCAL_LDFLAGS+= 

include $(BUILD_APP)
	
.PHONY: $(LOCAL_TARGET)

$(LOCAL_TARGET): PRIVATE_PATH:=$(LOCAL_PATH)
$(LOCAL_TARGET): $(LOCAL_MODULE)
	@echo "build cec_chime_control ======================="
	@cp -f $< $(CHICONY_OUT_BIN_DIR)/
	@echo "Build" $@ Done.

$(call add-target-into-build, $(LOCAL_TARGET))

endif
