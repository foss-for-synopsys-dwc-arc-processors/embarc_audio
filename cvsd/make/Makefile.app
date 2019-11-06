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

COMPONENT ?= CVSD_codec

CODEC_ID_PREFIX = arc_cvsd_codec
CODEC_ID_PREFIX_UPPER = ARC_CVSD_CODEC

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all run clean cleanall run run_decoder run_codec

#################################################
# Test app parameters section

APP_NAME = $(CODEC_ID_PREFIX_UPPER)_app.elf

# Setting target
all: $(APP_BUILD_DIR) $(APP_NAME)  

CLEAN_BINS += $(APP_NAME)
MAP_NAME   += $(EXECUTOR_NAME).map

APP_C_FILES = cvsd_example.c filter.c

include ..$(PS)..$(PS)rules$(PS)common_app.mk

#################################################
# Run application

ifneq (,$(TCF_FLAG))

run_decoder:
	@echo [Running decoder of application]
	mdb $(DBG_HWFLAGS) $(APP_NAME) -dec -F 256 -i "..$(PS)testvectors$(PS)inp$(PS)test_vector_64000.cod"  -o "..$(PS)testvectors$(PS)test_vector_8000.out"
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)test_vector_8000.out ..$(PS)testvectors$(PS)ref$(PS)test_vector_8000.out)

run_encoder:
	@echo [Running encoder of application]
	mdb $(DBG_HWFLAGS) $(APP_NAME) -enc -F 128 -i "..$(PS)testvectors$(PS)inp$(PS)test_vector_8000.raw"  -o "..$(PS)testvectors$(PS)test_vector_64000.cod"
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)test_vector_64000.cod ..$(PS)testvectors$(PS)ref$(PS)test_vector_64000.cod)

run_codec:
	@echo [Running transcoder of application] 
	mdb $(DBG_HWFLAGS) $(APP_NAME) -encdec -F 128 -i "..$(PS)testvectors$(PS)inp$(PS)test_vector_8000.raw"  -o "..$(PS)testvectors$(PS)test_vector_8000.out"
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)test_vector_8000.out ..$(PS)testvectors$(PS)ref$(PS)test_vector_8000.out)

run: run_codec run_decoder run_encoder

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
