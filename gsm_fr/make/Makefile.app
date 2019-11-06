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

CODEC_ID_PREFIX = arc_fr_codec
CODEC_ID_PREFIX_UPPER = ARC_FR_CODEC

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all run clean cleanall run run_decoder run_encoder

#################################################
# Test app parameters section

APP_NAME = $(CODEC_ID_PREFIX_UPPER)_app.elf

# Setting target
all: $(APP_BUILD_DIR) $(APP_NAME)  

CLEAN_BINS += $(APP_NAME)

APP_INCLUDE_DIRS = ..$(PS)test$(PS)tls

APP_C_FILES_DIRS = ..$(PS)test$(PS)tls \
                   ..$(PS)test$(PS)add-test

override APP_C_FILES += rpeltp.c \
                        rpedemo.c \
                        g711.c

APP_ALL_C_FILES += arc_profile.c  \
                   helper_lib.c \
                   rpeltp.c \
                   rpedemo.c \
                   g711.c

include ..$(PS)..$(PS)rules$(PS)common_app.mk

#################################################
# Run application

ifneq (,$(TCF_FLAG))

run_decoder:
	@echo [Running decoder of application]
	mdb $(DBG_HWFLAGS) $(APP_NAME) -l -dec  ..$(PS)testvectors$(PS)inp$(PS)Seq01.cod ..$(PS)testvectors$(PS)Seq01.out
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq01.out ..$(PS)testvectors$(PS)Seq01.out)

run_encoder:
	@echo [Running encoder of application]
	mdb $(DBG_HWFLAGS) $(APP_NAME) -l -enc ..$(PS)testvectors$(PS)inp$(PS)Seq01.inp ..$(PS)testvectors$(PS)Seq01.cod
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq01.cod ..$(PS)testvectors$(PS)Seq01.cod)

run_enc_dec:
	@echo [Running encoder of application]
	mdb $(DBG_HWFLAGS) $(APP_NAME) -l   ..$(PS)testvectors$(PS)inp$(PS)Seq01.inp ..$(PS)testvectors$(PS)Seq01.out
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq01.out ..$(PS)testvectors$(PS)Seq01.out)

run: run_encoder run_decoder  

run_all:
	@echo [Running encoder of application]
	@mdb $(DBG_HWFLAGS) $(APP_NAME) -l   ..$(PS)testvectors$(PS)inp$(PS)Seq01.inp ..$(PS)testvectors$(PS)Seq01.out
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq01.out ..$(PS)testvectors$(PS)Seq01.out)
	@mdb $(DBG_HWFLAGS) $(APP_NAME) -l   ..$(PS)testvectors$(PS)inp$(PS)Seq02.inp ..$(PS)testvectors$(PS)Seq02.out
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq02.out ..$(PS)testvectors$(PS)Seq02.out)
	@mdb $(DBG_HWFLAGS) $(APP_NAME) -l   ..$(PS)testvectors$(PS)inp$(PS)Seq03.inp ..$(PS)testvectors$(PS)Seq03.out
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq03.out ..$(PS)testvectors$(PS)Seq03.out)
	@mdb $(DBG_HWFLAGS) $(APP_NAME) -l   ..$(PS)testvectors$(PS)inp$(PS)Seq04.inp ..$(PS)testvectors$(PS)Seq04.out
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq04.out ..$(PS)testvectors$(PS)Seq04.out)
	@mdb $(DBG_HWFLAGS) $(APP_NAME) -l -dec  ..$(PS)testvectors$(PS)inp$(PS)Seq05.cod ..$(PS)testvectors$(PS)Seq05.out
	@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)Seq05.out ..$(PS)testvectors$(PS)Seq05.out)
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
