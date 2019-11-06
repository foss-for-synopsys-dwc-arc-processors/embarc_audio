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
CODEC_ID_PREFIX = arc_g726_codec
EXECUTOR ?= VBR_APP

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all lib clean cleanall app run demo vbr
all: lib app

#################################################
# Library parameters section

LIB_NAME = $(CODEC_ID_PREFIX)
SYMBOLS_FILE   = $(CODEC_ID_PREFIX)_symbols.txt
CODEC_LIB = $(BINARY_DIR)$(PS)$(LIB_NAME).lib

# Setting target
lib: $(BUILD_DIR) $(BINARY_DIR) $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a
CLEAN_BINS += $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a

LIB_C_FILES_OPT_SPEED_COMMON = g711.c \
                               g726.c

LIB_C_FILES_OPT_SIZE_COMMON = \

include ..$(PS)..$(PS)rules$(PS)common.mk
#################################################
# Application section

demo: lib 
	$(MAKE) -f Makefile.app CODEC_LIB=$(CODEC_LIB) EXECUTOR=CBR_APP
vbr: lib 
	$(MAKE) -f Makefile.app CODEC_LIB=$(CODEC_LIB) EXECUTOR=VBR_APP

app: demo vbr

#################################################
# Size evaluation
size: lib
	@echo [Dynamic library size]:
	@$(SZ) $(BINARY_DIR)$(PS)$(LIB_NAME).lib
	@echo [Static library size]:
	@$(SZ) -t $(BINARY_DIR)$(PS)$(LIB_NAME).a

#################################################
# Run codec
run: run_vbr run_cbr

run_encoder: run_encoder_cbr run_encoder_vbr

run_decoder: run_decoder_cbr run_decoder_vbr

run_codec: run_codec_cbr run_codec_vbr

run_cbr: demo
	$(MAKE) -f Makefile.app run CODEC_LIB=$(CODEC_LIB) EXECUTOR=CBR_APP

run_encoder_cbr: demo
	$(MAKE) -f Makefile.app run_encoder CODEC_LIB=$(CODEC_LIB) EXECUTOR=CBR_APP

run_decoder_cbr: demo
	$(MAKE) -f Makefile.app run_decoder CODEC_LIB=$(CODEC_LIB) EXECUTOR=CBR_APP

run_codec_cbr: demo
	$(MAKE) -f Makefile.app run_codec CODEC_LIB=$(CODEC_LIB) EXECUTOR=CBR_APP

run_vbr: vbr
	$(MAKE) -f Makefile.app run CODEC_LIB=$(CODEC_LIB) EXECUTOR=VBR_APP

run_encoder_vbr: vbr
	$(MAKE) -f Makefile.app run_encoder CODEC_LIB=$(CODEC_LIB) EXECUTOR=VBR_APP

run_decoder_vbr: vbr
	$(MAKE) -f Makefile.app run_decoder CODEC_LIB=$(CODEC_LIB) EXECUTOR=VBR_APP

run_codec_vbr: vbr
	$(MAKE) -f Makefile.app run_codec CODEC_LIB=$(CODEC_LIB) EXECUTOR=VBR_APP

run_all_tests: demo vbr
	$(MAKE) -f Makefile.app check_all_tests CODEC_LIB=$(CODEC_LIB) EXECUTOR=$(EXECUTOR)

##################################################
# Global VPATH settings

vpath %.c   $(LIB_C_FILES_DIRS)
vpath %.h   $(LIB_INCLUDE_DIRS)

clean: $(BUILD_DIR)
	@echo [Deleting OBJ files]
	@$(RM) $(call fix_platform_path, $(CLEAN_OBJS))
	@echo [Deleting dependency files]
	@$(RM) $(call fix_platform_path, $(CLEAN_DEPENDS))
	@$(MAKE) -f Makefile.app clean EXECUTOR=VBR_APP
	@$(MAKE) -f Makefile.app clean EXECUTOR=CBR_APP

cleanall: $(BINARY_DIR) clean
	@echo [Deleting binary files]
	@$(RM) $(call fix_platform_path, $(CLEAN_BINS))
	@$(MAKE) -f Makefile.app cleanall EXECUTOR=VBR_APP
	@$(MAKE) -f Makefile.app cleanall EXECUTOR=CBR_APP
