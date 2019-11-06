#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

#################################################
# Global Settings

CODEC_ID_PREFIX = arc_cvsd_codec

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all lib clean cleanall app run run_decoder run_encoder

all: lib app 

#################################################
# Library parameters section

LIB_NAME     = $(CODEC_ID_PREFIX)
SYMBOLS_FILE = $(CODEC_ID_PREFIX)_symbols.txt

# Setting target
lib: $(BUILD_DIR) $(BINARY_DIR) $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a
CLEAN_BINS += $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a

LIB_C_FILES_OPT_SIZE_COMMON  = 
LIB_C_FILES_OPT_SPEED_COMMON += cvsd.c

include ..$(PS)..$(PS)rules$(PS)common.mk
#################################################
# Application section
app: lib
	$(MAKE) -f Makefile.app CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).lib

#################################################
# Run application
run_decoder: app
	$(MAKE) -f Makefile.app run_decoder CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).lib
	
run_codec: app
	$(MAKE) -f Makefile.app run_codec CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).lib

run_encoder:app
	$(MAKE) -f Makefile.app run_encoder CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).lib
run: run_decoder run_encoder run_codec

#################################################
# Size evaluation

size: lib
	@echo [Dynamic library size]:
	@$(SZ) $(BINARY_DIR)$(PS)$(LIB_NAME).lib
	@echo [Static library size]:
	@$(SZ) -t $(BINARY_DIR)$(PS)$(LIB_NAME).a

##################################################
# Global VPATH settings

vpath %.c   $(LIB_C_FILES_DIRS)
vpath %.h   $(LIB_INCLUDE_DIRS)

clean: $(BUILD_DIR)
	@echo [Deleting OBJ files]
	@$(RM) $(call fix_platform_path, $(CLEAN_OBJS))
	@echo [Deleting dependency files]
	@$(RM) $(call fix_platform_path, $(CLEAN_DEPENDS))
	@$(MAKE) -f Makefile.app clean 

cleanall: clean $(BINARY_DIR)
	@echo [Deleting binary files]
	@$(RM) $(call fix_platform_path, $(CLEAN_BINS))
	@$(MAKE) -f Makefile.app cleanall