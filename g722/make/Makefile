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
CODEC_ID_PREFIX = arc_g722IV_codec
#  For encoder and decoder builds separated libs
MULTI_LIBS_MODE = on
#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all lib lib_dec lib_enc clean cleanall app run app_encoder app_decoder run_encoder run_decoder
all: lib app

#################################################
# Library parameters section

LIB_NAME     = $(CODEC_ID_PREFIX)
SYMBOLS_FILE_ENC = arc_g722IV_enc_symbols.txt
SYMBOLS_FILE_DEC = arc_g722IV_dec_symbols.txt
FULL_LIB_NAME_ENC = $(BINARY_DIR)$(PS)arc_g722IV_enc
FULL_LIB_NAME_DEC = $(BINARY_DIR)$(PS)arc_g722IV_dec
DECODER_LIB = $(FULL_LIB_NAME_DEC).lib
ENCODER_LIB = $(FULL_LIB_NAME_ENC).lib

# Setting target
lib_enc: $(BUILD_DIR) $(BINARY_DIR) $(FULL_LIB_NAME_ENC).lib $(FULL_LIB_NAME_ENC).a
lib_dec: $(BUILD_DIR) $(BINARY_DIR) $(FULL_LIB_NAME_DEC).lib $(FULL_LIB_NAME_DEC).a
CLEAN_BINS += $(FULL_LIB_NAME_DEC).lib $(FULL_LIB_NAME_DEC).a \
              $(FULL_LIB_NAME_ENC).lib $(FULL_LIB_NAME_ENC).a

ifeq ($(COMPONENT),ENCODER)
lib: lib_enc
else
ifeq ($(COMPONENT),DECODER)
lib: lib_dec
else
lib: lib_dec lib_enc
endif
endif

ifneq ($(REMAPITU_T),on)
LIB_C_FILES_OPT_SPEED_COMMON += basop32_.c \
                                enh1632.c \
                                control.c \
                                count.c
endif

override LIB_INCLUDE_DIRS_COMMON += ..$(PS)src$(PS)stl2005_basop

override LIB_C_FILES_DIRS_COMMON += ..$(PS)src$(PS)stl2005_basop

LIB_C_FILES_OPT_SPEED_COMMON += funcg722.c\
                                g722.c \
                                oper_32b.c \
                                g722_plc.c \
                                g722_plc_tables.c \
                                softbit.c


LIB_C_FILES_OPT_SIZE_COMMON = \

include ..$(PS)..$(PS)rules$(PS)common.mk
#################################################
# Application section
app_encoder: lib_enc
	$(MAKE) -f Makefile.app CODEC_LIB=$(ENCODER_LIB) COMPONENT=ENCODER

app_decoder: lib_dec
	$(MAKE) -f Makefile.app CODEC_LIB=$(DECODER_LIB) COMPONENT=DECODER

tstcg722:
	$(MAKE) -f Makefile.app CODEC_LIB=$(ENCODER_LIB) COMPONENT=TSTCG722

tstdg722:
	$(MAKE) -f Makefile.app CODEC_LIB=$(DECODER_LIB) COMPONENT=TSTDG722

ifeq ($(COMPONENT),ENCODER)
app: app_encoder tstcg722
else
ifeq ($(COMPONENT),DECODER)
app: app_decoder tstdg722
else
app: app_decoder app_encoder tst
endif
endif

tst: tstcg722 tstdg722

#################################################
# Size evaluation
size: lib
	@echo [Dynamic library size $(ENCODER_LIB)]:
	@$(SZ) $(ENCODER_LIB)
	@echo [Dynamic library size $(DECODER_LIB)]:
	@$(SZ) $(DECODER_LIB)

#################################################
# Run codec
run: run_encoder run_decoder

run_encoder: app_encoder
	$(MAKE) -f Makefile.app run_encoder CODEC_LIB=$(ENCODER_LIB) COMPONENT=ENCODER

run_decoder: app_decoder
	$(MAKE) -f Makefile.app run_decoder CODEC_LIB=$(DECODER_LIB) COMPONENT=DECODER

run_tstcg722: tstcg722
	$(MAKE) -f Makefile.app run_tstcg722 CODEC_LIB=$(ENCODER_LIB) COMPONENT=TSTCG722

run_tstdg722: tstdg722
	$(MAKE) -f Makefile.app run_tstdg722 CODEC_LIB=$(ENCODER_LIB) COMPONENT=TSTDG722
##################################################
# Global VPATH settings

vpath %.c   $(LIB_C_FILES_DIRS)
vpath %.h   $(LIB_INCLUDE_DIRS)

clean: $(BUILD_DIR)
	@echo [Deleting OBJ files]
	@$(RM) $(call fix_platform_path, $(CLEAN_OBJS))
	@echo [Deleting dependency files]
	@$(RM) $(call fix_platform_path, $(CLEAN_DEPENDS))
	@$(MAKE) -f Makefile.app clean COMPONENT=ENCODER
	@$(MAKE) -f Makefile.app clean COMPONENT=DECODER

cleanall: $(BINARY_DIR) clean
	@echo [Deleting binary files]
	@$(RM) $(call fix_platform_path, $(CLEAN_BINS))
	@$(MAKE) -f Makefile.app cleanall COMPONENT=ENCODER
	@$(MAKE) -f Makefile.app cleanall COMPONENT=DECODER
