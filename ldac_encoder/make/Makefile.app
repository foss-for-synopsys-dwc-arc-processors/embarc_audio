#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

#################################################
# Global parameters section
#
#  CODEC_ID_PREFIX - target codec <ID> prefix

CODEC_ID_PREFIX = ldac_encoder
CODEC_ID_PREFIX_UPPER = LDAC_ENCODER

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all clean cleanall run run_encoder

#################################################
# Test app parameters section


APP_NAME = $(CODEC_ID_PREFIX_UPPER)_app.elf

# Setting target
all: $(APP_BUILD_DIR) $(APP_NAME)  

CLEAN_BINS += $(APP_NAME)


APP_C_FILES = main.c	\
			  wavlib.c	\
			  wrapalloc.c
			
include ..$(PS)..$(PS)rules$(PS)common_app.mk

#################################################
# Run application

ifneq (,$(TCF_FLAG))

run_encoder:
	@echo [Running encoder on example_96k_2byte_2ch_FS110]
	mdb $(DBG_HWFLAGS) $(APP_NAME) -i ..$(PS)testvectors$(PS)inp$(PS)example_96k_2byte_2ch.wav -o ..$(PS)testvectors$(PS)example_96k_2byte_2ch_FS110.bin -F 110
	@echo [Check file]
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)example_96k_2byte_2ch_FS110.bin ..$(PS)testvectors$(PS)example_96k_2byte_2ch_FS110.bin)

run_encoder_debug:
	@echo [Running encoder on example_96k_2byte_2ch_FS110]
	mdb $(DBG_HWFLAGS) $(APP_NAME) -i ..$(PS)testvectors$(PS)inp$(PS)example_96k_2byte_2ch.wav -o ..$(PS)testvectors$(PS)example_96k_2byte_2ch_FS110.bin -F 110
	@echo [Check file]
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)example_96k_2byte_2ch_FS110.bin ..$(PS)testvectors$(PS)example_96k_2byte_2ch_FS110.bin)

run: run_encoder
endif

##################################################
# Global VPATH settings

vpath %.c   $(APP_C_FILES_DIRS)
vpath %.h   $(APP_INCLUDE_DIRS)

clean: $(APP_BUILD_DIR)
	@echo [Deleting OBJ files]
	@$(RM) $(call fix_platform_path, $(CLEAN_OBJS))

cleanall: clean
	@echo [Deleting binary files]
	@$(RM) $(call fix_platform_path, $(CLEAN_BINS))
	@$(RM) $(call fix_platform_path, $(call get_file_name, $(APP_NAME)).map)
