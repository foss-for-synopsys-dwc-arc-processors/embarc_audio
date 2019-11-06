#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

# Specific C flags
override CFLAGS += -Hnocopyr -Hnocplus
override APP_CFLAGS += -Hpurge -Hsdata0
override LDFLAGS += -Hldopt=-q,-m,-Csymbols,-Coutput=$(call get_file_name, $@).map

ifeq ($(REMAPITU_T),on)
override CFLAGS += -Xdsp_ctrl=noguard,up,preshift -Hitut -Hfxapi
else
override CFLAGS += -Xdsp_ctrl=noguard,up -fno-builtin-round
endif

ifeq ($(NATIVE_CYCLE_PROFILING),on)
override APP_C_FILES += arc_profile.c
endif
override APP_C_FILES += helper_lib.c

# Internal definitions (do not modify)
override APP_INCLUDE_DIRS += ..$(PS)include \
                             ..$(PS)..$(PS)testlib$(PS)include \
                             ..$(PS)test$(PS)include

override APP_C_FILES_DIRS += ..$(PS)..$(PS)testlib \
                             ..$(PS)test

APP_INCS     = $(foreach dir,$(APP_INCLUDE_DIRS), $(addprefix -I,$(dir)))
APP_OBJS_C   = $(addprefix $(APP_BUILD_DIR)$(PS), $(APP_C_FILES:.c=.o))
APP_OBJS_ALL_C   = $(addprefix $(APP_BUILD_DIR)$(PS), $(APP_ALL_C_FILES:.c=.o))

CLEAN_OBJS += $(addprefix $(APP_BUILD_DIR)$(PS), $(APP_C_FILES:.c=.o))
override CLEAN_OBJS += $(APP_OBJS_ALL_C)

$(APP_BUILD_DIR):
	@echo [MKDIR] [Creating directory $@]
	@$(MKDIR) $(call fix_platform_path, $(APP_BUILD_DIR))

$(APP_NAME): $(APP_OBJS_C) $(CODEC_LIB) $(LIBS_EXTRA)
	@echo [LD] [$(BUILD_TYPE_MSG) build] [Linking ELF output binary $@] 
	@$(LD) $(LDFLAGS) $(CFLAGS) $(APP_CFLAGS) $(FLAGS_OPT_SIZE) -o $@ $^

$(APP_OBJS_C): $(APP_BUILD_DIR)$(PS)%.o: %.c
	@echo Compiling C [opt SIZE]: $(CC) $< ...
	@$(CC) $(APP_INCS) $(CFLAGS) $(APP_CFLAGS) $(FLAGS_OPT_SIZE) -c $< -o $@

##################################################
# Global VPATH settings

vpath %.c   $(APP_C_FILES_DIRS)
vpath %.h   $(APP_INCLUDE_DIRS)

#################################################
# Run application

TCF_FLAG=$(filter -tcf=%, $(CFLAGS))
DBG_HWFLAGS = -cl  -run -nohostlink_while_running -nsim -notrace -nooptions -profile -on=nsim_bypass_option_sanity_check -on=cycles  $(TCF_FLAG)
DBG_GUI_HWFLAGS = -gui -nooptions -noproject -OKN -nsim $(TCF_FLAG)

ifeq (,$(TCF_FLAG))

$(warning [WARNING] TCF file name not found in CFLAGS, make target "run" are disabled)
$(info CFLAGS=$(CFLAGS))
$(info [INFO] It is recommended to use TCF file instead of low level options like -Xbs)
$(info [INFO] You can add string like)
$(info [INFO]     override CFLAGS += -tcf=<TCF_FILE_NAME> -tcf_core_config)
$(info [INFO] to hw_config.mk file)
$(info [INFO] See FLAC Decoder Databook for details on using TCF files)

.PHONY: run  run_decoder run_encoder

run run_decoder run_encoder:
	$(error [ERROR] TCF file name not found in CFLAGS, make targets "run" is disabled)

endif
