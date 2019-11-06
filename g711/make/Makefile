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
CODEC_ID_PREFIX = arc_g711_codec

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all lib clean cleanall app run
all: lib app

#################################################
# Library parameters section

LIB_NAME = $(CODEC_ID_PREFIX)
SYMBOLS_FILE   = $(CODEC_ID_PREFIX)_symbols.txt

# Setting target
lib: $(BUILD_DIR) $(BINARY_DIR) $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a
CLEAN_BINS += $(BINARY_DIR)$(PS)$(LIB_NAME).lib $(BINARY_DIR)$(PS)$(LIB_NAME).a

LIB_C_FILES_OPT_SPEED_COMMON = \
g711.c \

LIB_C_FILES_OPT_SIZE_COMMON = \

include ..$(PS)..$(PS)rules$(PS)common.mk
#################################################
# Application section
app: lib
	$(MAKE) -f Makefile.app CODEC_LIB=$(BINARY_DIR)$(PS)$(LIB_NAME).lib

#################################################
# Size evaluation

size: lib
	@echo [Dynamic library size]:
	@$(SZ) $(BINARY_DIR)$(PS)$(LIB_NAME).lib
	@echo [Static library size]:
	@$(SZ) -t $(BINARY_DIR)$(PS)$(LIB_NAME).a

run: app
	$(MAKE) -f Makefile.app run

run_a_encoder: app
	$(MAKE) -f Makefile.app run_a_encoder

run_a_decoder: app
	$(MAKE) -f Makefile.app run_a_decoder

run_a_codec: app
	$(MAKE) -f Makefile.app run_a_codec

run_mu_encoder: app
	$(MAKE) -f Makefile.app run_mu_encoder

run_mu_decoder: app
	$(MAKE) -f Makefile.app run_mu_decoder

run_mu_codec: app
	$(MAKE) -f Makefile.app run_mu_codec
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

cleanall: $(BINARY_DIR) clean
	@echo [Deleting binary files]
	@$(RM) $(call fix_platform_path, $(CLEAN_BINS))
	@$(MAKE) -f Makefile.app cleanall
