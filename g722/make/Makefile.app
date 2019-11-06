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

ifeq ($(COMPONENT),ENCODER)
EXECUTOR_NAME = encg722
endif
ifeq ($(COMPONENT),DECODER)
EXECUTOR_NAME = decg722
endif
ifeq ($(COMPONENT),TSTCG722)
EXECUTOR_NAME = tstcg722
endif
ifeq ($(COMPONENT),TSTDG722)
EXECUTOR_NAME = tstdg722
endif

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all run run_encoder run_decoder clean cleanall 

APP_NAME = $(EXECUTOR_NAME).elf

# Setting target
all: $(APP_BUILD_DIR) $(APP_NAME)  

CLEAN_BINS += tstcg722.elf tstdg722.elf encg722.elf decg722.elf
MAP_NAME   += tstcg722.map tstdg722.map encg722.map decg722.map

APP_INCLUDE_DIRS = ..$(PS)src$(PS)stl2005_basop

APP_C_FILES_DIRS = ..$(PS)src$(PS)stl2005_basop

APP_C_FILES =	$(EXECUTOR_NAME).c

override CFLAGS += -Hstack=96K
ifneq ($(REMAPITU_T),on)
APP_C_FILES +=	basop32_.c
endif

# Internal definitions (do not modify)
APP_C_FILES_ALL = $(APP_C_FILES) tstcg722.c tstdg722.c encg722.c decg722.c

include ..$(PS)..$(PS)rules$(PS)common_app.mk
##################################################
# Running test vector

ifneq (,$(TCF_FLAG))

run_decoder: all
	@echo Running test vector test10.bst ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -fsize 160 ..$(PS)testvectors$(PS)inp$(PS)test10.bst ..$(PS)testvectors$(PS)test10.pout
	$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)test10.out ..$(PS)testvectors$(PS)test10.pout)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)test10.pout)

run_encoder: all
	@echo Running test vector inpsp.bin ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -byte ..$(PS)testvectors$(PS)inp$(PS)inpsp.bin ..$(PS)testvectors$(PS)temp.cod
	$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)codspw.cod ..$(PS)testvectors$(PS)temp.cod)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)temp.cod)

run_tstdg722:
	@echo Running coverage tests of decoder ...
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt2r1.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l1.rc1 ..$(PS)testvectors$(PS)ref$(PS)bt3h1.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt2r1.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l1.rc2 ..$(PS)testvectors$(PS)ref$(PS)bt3h1.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt2r1.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l1.rc3 ..$(PS)testvectors$(PS)ref$(PS)bt3h1.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt2r2.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l2.rc1 ..$(PS)testvectors$(PS)ref$(PS)bt3h2.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt2r2.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l2.rc2 ..$(PS)testvectors$(PS)ref$(PS)bt3h2.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt2r2.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l2.rc3 ..$(PS)testvectors$(PS)ref$(PS)bt3h2.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt1d3.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l3.rc1 ..$(PS)testvectors$(PS)ref$(PS)bt3h3.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt1d3.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l3.rc2 ..$(PS)testvectors$(PS)ref$(PS)bt3h3.rc0 
	mdb $(DBG_HWFLAGS) tstdg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt1d3.cod ..$(PS)testvectors$(PS)ref$(PS)bt3l3.rc3 ..$(PS)testvectors$(PS)ref$(PS)bt3h3.rc0 

run_tstcg722:
	@echo Running coverage tests of encoder ...
	mdb $(DBG_HWFLAGS) tstcg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt1c1.xmt ..$(PS)testvectors$(PS)ref$(PS)bt2r1.cod
	mdb $(DBG_HWFLAGS) tstcg722.elf ..$(PS)testvectors$(PS)inp$(PS)bt1c2.xmt ..$(PS)testvectors$(PS)ref$(PS)bt2r2.cod

run: run_encoder run_encoder

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
	@$(RM) $(call fix_platform_path, $(MAP_NAME))
