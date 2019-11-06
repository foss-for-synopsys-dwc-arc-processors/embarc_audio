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

ifeq ($(EXECUTOR),VBR_APP)
EXECUTOR_NAME = vbr-g726
endif
ifeq ($(EXECUTOR),CBR_APP)
EXECUTOR_NAME = g726demo
endif

#################################################
# Include external configuration files
include ../../rules/init.mk

#################################################
# Targets list

.PHONY: all run clean cleanall 

APP_NAME = $(EXECUTOR_NAME).elf

# Setting target
all: $(APP_BUILD_DIR) $(APP_NAME)  

CLEAN_BINS += $(APP_NAME)
MAP_NAME   += $(EXECUTOR_NAME).map

APP_C_FILES = $(EXECUTOR_NAME).c

include ..$(PS)..$(PS)rules$(PS)common_app.mk

#################################################
# Run application
ifneq (,$(TCF_FLAG))
ifeq ($(EXECUTOR),VBR_APP)
run: run_codec run_encoder run_decoder

run_codec: all
	@echo Running test vector voice ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law A -rate 16-24-32-40-32-24 ..$(PS)testvectors$(PS)inp$(PS)voice.src ..$(PS)testvectors$(PS)voicvbra.tst
	$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)voicevbr.arf ..$(PS)testvectors$(PS)voicvbra.tst)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)voicvbra.tst)

run_encoder: all
	@echo Running test vector nrm ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 16 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn16fa.i 16 1 1024
	$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fa.i)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)rn16fa.i)

run_decoder: all
	@echo Running test vector nrm ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fa.i ..$(PS)testvectors$(PS)rv16fa.o 16 1 128
	$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fa.o ..$(PS)testvectors$(PS)rv16fa.o)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)rv16fa.o)

run_all_tests: all
	@echo Running test vectors $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law A -rate 16-24-32-40-32-24 ..$(PS)testvectors$(PS)inp$(PS)voice.src ..$(PS)testvectors$(PS)voicvbra.tst
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law l -rate 16-24-32-40-32-24 ..$(PS)testvectors$(PS)inp$(PS)voice.src ..$(PS)testvectors$(PS)voicvbrl.tst
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -rate 16-24-32-40-32-24 ..$(PS)testvectors$(PS)inp$(PS)voice.src ..$(PS)testvectors$(PS)voicvbru.tst
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 16 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn16fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 16 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv16fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fa.i ..$(PS)testvectors$(PS)rv16fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fa.i ..$(PS)testvectors$(PS)rv16fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 16 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn16fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 16 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv16fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fm.i ..$(PS)testvectors$(PS)rn16fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fm.i ..$(PS)testvectors$(PS)rv16fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fm.i ..$(PS)testvectors$(PS)rn16fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fm.i ..$(PS)testvectors$(PS)rv16fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)i16 ..$(PS)testvectors$(PS)ri16fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 16 ..$(PS)testvectors$(PS)inp$(PS)i16 ..$(PS)testvectors$(PS)ri16fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 24 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn24fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 24 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv24fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fa.i ..$(PS)testvectors$(PS)rn24fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fa.i ..$(PS)testvectors$(PS)rv24fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fa.i ..$(PS)testvectors$(PS)rn24fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fa.i ..$(PS)testvectors$(PS)rv24fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 24 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn24fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 24 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv24fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fm.i ..$(PS)testvectors$(PS)rn24fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fm.i ..$(PS)testvectors$(PS)rv24fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fm.i ..$(PS)testvectors$(PS)rn24fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fm.i ..$(PS)testvectors$(PS)rv24fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)i24 ..$(PS)testvectors$(PS)ri24fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 24 ..$(PS)testvectors$(PS)inp$(PS)i24 ..$(PS)testvectors$(PS)ri24fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 32 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn32fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 32 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv32fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fa.i ..$(PS)testvectors$(PS)rn32fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fa.i ..$(PS)testvectors$(PS)rv32fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fa.i ..$(PS)testvectors$(PS)rn32fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fa.i ..$(PS)testvectors$(PS)rv32fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 32 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn32fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 32 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv32fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fm.i ..$(PS)testvectors$(PS)rn32fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fm.i ..$(PS)testvectors$(PS)rv32fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fm.i ..$(PS)testvectors$(PS)rn32fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fm.i ..$(PS)testvectors$(PS)rv32fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)i32 ..$(PS)testvectors$(PS)ri32fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 32 ..$(PS)testvectors$(PS)inp$(PS)i32 ..$(PS)testvectors$(PS)ri32fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 40 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn40fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -enc -rate 40 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv40fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fa.i ..$(PS)testvectors$(PS)rn40fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fa.i ..$(PS)testvectors$(PS)rv40fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fa.i ..$(PS)testvectors$(PS)rn40fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fa.i ..$(PS)testvectors$(PS)rv40fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 40 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn40fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -enc -rate 40 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv40fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fm.i ..$(PS)testvectors$(PS)rn40fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fm.i ..$(PS)testvectors$(PS)rv40fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fm.i ..$(PS)testvectors$(PS)rn40fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fm.i ..$(PS)testvectors$(PS)rv40fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law a -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)i40 ..$(PS)testvectors$(PS)ri40fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q -law u -dec -rate 40 ..$(PS)testvectors$(PS)inp$(PS)i40 ..$(PS)testvectors$(PS)ri40fm.o 16 1 1024

else
run: run_codec run_encoder run_decoder

run_codec: all
	@echo Running test vector $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 16 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn16fa.i 16 1 1024
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fa.i) > nul && echo "\033[92m"BITWISE OK"\033[0m" || echo "\033[91m"BITWISE BROKEN"\033[0m"
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)rn16fa.i)

run_encoder: all
	@echo Running test vector nrm ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 16 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn16fa.i 16 1 1024
	$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fa.i)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)rn16fa.i)

run_decoder: all
	@echo Running test vector nrm ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fa.i ..$(PS)testvectors$(PS)rv16fa.o 16 1 128
	$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fa.o ..$(PS)testvectors$(PS)rv16fa.o)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)rv16fa.o)

run_all_tests: all
	@echo Running test vectors $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 16 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn16fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 16 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv16fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fa.i ..$(PS)testvectors$(PS)rv16fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fa.i ..$(PS)testvectors$(PS)rv16fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 16 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn16fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 16 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv16fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fm.i ..$(PS)testvectors$(PS)rn16fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fm.i ..$(PS)testvectors$(PS)rv16fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rn16fm.i ..$(PS)testvectors$(PS)rn16fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 16 ..$(PS)testvectors$(PS)inp$(PS)rv16fm.i ..$(PS)testvectors$(PS)rv16fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 16 ..$(PS)testvectors$(PS)inp$(PS)i16 ..$(PS)testvectors$(PS)ri16fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 16 ..$(PS)testvectors$(PS)inp$(PS)i16 ..$(PS)testvectors$(PS)ri16fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 24 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn24fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 24 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv24fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fa.i ..$(PS)testvectors$(PS)rn24fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fa.i ..$(PS)testvectors$(PS)rv24fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fa.i ..$(PS)testvectors$(PS)rn24fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fa.i ..$(PS)testvectors$(PS)rv24fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 24 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn24fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 24 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv24fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fm.i ..$(PS)testvectors$(PS)rn24fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fm.i ..$(PS)testvectors$(PS)rv24fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rn24fm.i ..$(PS)testvectors$(PS)rn24fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 24 ..$(PS)testvectors$(PS)inp$(PS)rv24fm.i ..$(PS)testvectors$(PS)rv24fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 24 ..$(PS)testvectors$(PS)inp$(PS)i24 ..$(PS)testvectors$(PS)ri24fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 24 ..$(PS)testvectors$(PS)inp$(PS)i24 ..$(PS)testvectors$(PS)ri24fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 40 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn40fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 40 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv40fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fa.i ..$(PS)testvectors$(PS)rn40fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fa.i ..$(PS)testvectors$(PS)rv40fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fa.i ..$(PS)testvectors$(PS)rn40fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fa.i ..$(PS)testvectors$(PS)rv40fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 40 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn40fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 40 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv40fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fm.i ..$(PS)testvectors$(PS)rn40fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fm.i ..$(PS)testvectors$(PS)rv40fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rn40fm.i ..$(PS)testvectors$(PS)rn40fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 40 ..$(PS)testvectors$(PS)inp$(PS)rv40fm.i ..$(PS)testvectors$(PS)rv40fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 40 ..$(PS)testvectors$(PS)inp$(PS)i40 ..$(PS)testvectors$(PS)ri40fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 40 ..$(PS)testvectors$(PS)inp$(PS)i40 ..$(PS)testvectors$(PS)ri40fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 32 ..$(PS)testvectors$(PS)inp$(PS)nrm.a ..$(PS)testvectors$(PS)rn32fa.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a load 32 ..$(PS)testvectors$(PS)inp$(PS)ovr.a ..$(PS)testvectors$(PS)rv32fa.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fa.i ..$(PS)testvectors$(PS)rn32fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fa.i ..$(PS)testvectors$(PS)rv32fa.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fa.i ..$(PS)testvectors$(PS)rn32fx.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fa.i ..$(PS)testvectors$(PS)rv32fx.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 32 ..$(PS)testvectors$(PS)inp$(PS)nrm.m ..$(PS)testvectors$(PS)rn32fm.i 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u load 32 ..$(PS)testvectors$(PS)inp$(PS)ovr.m ..$(PS)testvectors$(PS)rv32fm.i 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fm.i ..$(PS)testvectors$(PS)rn32fm.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fm.i ..$(PS)testvectors$(PS)rv32fm.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rn32fm.i ..$(PS)testvectors$(PS)rn32fc.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 32 ..$(PS)testvectors$(PS)inp$(PS)rv32fm.i ..$(PS)testvectors$(PS)rv32fc.o 16 1 128
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q a adlo 32 ..$(PS)testvectors$(PS)inp$(PS)i32 ..$(PS)testvectors$(PS)ri32fa.o 16 1 1024
	mdb $(DBG_HWFLAGS) $(APP_NAME) -q u adlo 32 ..$(PS)testvectors$(PS)inp$(PS)i32 ..$(PS)testvectors$(PS)ri32fm.o 16 1 1024

endif

check_all_tests: run_all_tests
ifeq ($(EXECUTOR),VBR_APP)
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)voicevbr.arf ..$(PS)testvectors$(PS)voicvbra.tst) > nul && echo "\033[92m"voicvbra.tst - BITWISE OK"\033[0m" || echo "\033[91m"voicvbra.tst - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)voicevbr.urf ..$(PS)testvectors$(PS)voicvbru.tst) > nul && echo "\033[92m"voicvbru.tst - BITWISE OK"\033[0m" || echo "\033[91m"voicvbru.tst - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)voicevbr.lrf ..$(PS)testvectors$(PS)voicvbrl.tst) > nul && echo "\033[92m"voicvbrl.tst - BITWISE OK"\033[0m" || echo "\033[91m"voicvbrl.tst - BITWISE BROKEN"\033[0m"
endif
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fa.i ..$(PS)testvectors$(PS)rn16fa.i) > nul && echo "\033[92m"rn16fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rn16fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fa.i ..$(PS)testvectors$(PS)rv16fa.i) > nul && echo "\033[92m"rv16fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rv16fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fa.o ..$(PS)testvectors$(PS)rn16fa.o) > nul && echo "\033[92m"rn16fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rn16fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fa.o ..$(PS)testvectors$(PS)rv16fa.o) > nul && echo "\033[92m"rv16fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rv16fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fx.o ..$(PS)testvectors$(PS)rn16fx.o) > nul && echo "\033[92m"rn16fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rn16fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fx.o ..$(PS)testvectors$(PS)rv16fx.o) > nul && echo "\033[92m"rv16fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rv16fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fc.o ..$(PS)testvectors$(PS)rn16fc.o) > nul && echo "\033[92m"rn16fc.o - BITWISE OK"\033[0m" || echo "\033[91m"rn16fc.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fc.o ..$(PS)testvectors$(PS)rv16fc.o) > nul && echo "\033[92m"rv16fc.o - BITWISE OK"\033[0m" || echo "\033[91m"rv16fc.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fm.o ..$(PS)testvectors$(PS)rn16fm.o) > nul && echo "\033[92m"rn16fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rn16fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fm.o ..$(PS)testvectors$(PS)rv16fm.o) > nul && echo "\033[92m"rv16fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rv16fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn16fm.i ..$(PS)testvectors$(PS)rn16fm.i) > nul && echo "\033[92m"rn16fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rn16fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv16fm.i ..$(PS)testvectors$(PS)rv16fm.i) > nul && echo "\033[92m"rv16fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rv16fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri16fa.o ..$(PS)testvectors$(PS)ri16fa.o) > nul && echo "\033[92m"ri16fa.o - BITWISE OK"\033[0m" || echo "\033[91m"ri16fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri16fm.o ..$(PS)testvectors$(PS)ri16fm.o) > nul && echo "\033[92m"ri16fm.o - BITWISE OK"\033[0m" || echo "\033[91m"ri16fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn24fa.i ..$(PS)testvectors$(PS)rn24fa.i) > nul && echo "\033[92m"rn24fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rn24fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv24fa.i ..$(PS)testvectors$(PS)rv24fa.i) > nul && echo "\033[92m"rv24fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rv24fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn24fa.o ..$(PS)testvectors$(PS)rn24fa.o) > nul && echo "\033[92m"rn24fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rn24fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv24fa.o ..$(PS)testvectors$(PS)rv24fa.o) > nul && echo "\033[92m"rv24fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rv24fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn24fx.o ..$(PS)testvectors$(PS)rn24fx.o) > nul && echo "\033[92m"rn24fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rn24fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv24fx.o ..$(PS)testvectors$(PS)rv24fx.o) > nul && echo "\033[92m"rv24fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rv24fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn24fc.o ..$(PS)testvectors$(PS)rn24fc.o) > nul && echo "\033[92m"rn24fc.o - BITWISE OK"\033[0m" || echo "\033[91m"rn24fc.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv24fc.o ..$(PS)testvectors$(PS)rv24fc.o) > nul && echo "\033[92m"rv24fc.o - BITWISE OK"\033[0m" || echo "\033[91m"rv24fc.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn24fm.o ..$(PS)testvectors$(PS)rn24fm.o) > nul && echo "\033[92m"rn24fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rn24fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv24fm.o ..$(PS)testvectors$(PS)rv24fm.o) > nul && echo "\033[92m"rv24fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rv24fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn24fm.i ..$(PS)testvectors$(PS)rn24fm.i) > nul && echo "\033[92m"rn24fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rn24fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv24fm.i ..$(PS)testvectors$(PS)rv24fm.i) > nul && echo "\033[92m"rv24fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rv24fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri24fa.o ..$(PS)testvectors$(PS)ri24fa.o) > nul && echo "\033[92m"ri24fa.o - BITWISE OK"\033[0m" || echo "\033[91m"ri24fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri24fm.o ..$(PS)testvectors$(PS)ri24fm.o) > nul && echo "\033[92m"ri24fm.o - BITWISE OK"\033[0m" || echo "\033[91m"ri24fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn32fa.i ..$(PS)testvectors$(PS)rn32fa.i) > nul && echo "\033[92m"rn32fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rn32fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv32fa.i ..$(PS)testvectors$(PS)rv32fa.i) > nul && echo "\033[92m"rv32fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rv32fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn32fa.o ..$(PS)testvectors$(PS)rn32fa.o) > nul && echo "\033[92m"rn32fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rn32fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv32fa.o ..$(PS)testvectors$(PS)rv32fa.o) > nul && echo "\033[92m"rv32fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rv32fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn32fx.o ..$(PS)testvectors$(PS)rn32fx.o) > nul && echo "\033[92m"rn32fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rn32fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv32fx.o ..$(PS)testvectors$(PS)rv32fx.o) > nul && echo "\033[92m"rv32fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rv32fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn32fc.o ..$(PS)testvectors$(PS)rn32fc.o) > nul && echo "\033[92m"rn32fc.o - BITWISE OK"\033[0m" || echo "\033[91m"rn32fc.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv32fc.o ..$(PS)testvectors$(PS)rv32fc.o) > nul && echo "\033[92m"rv32fc.o - BITWISE OK"\033[0m" || echo "\033[91m"rv32fc.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn32fm.o ..$(PS)testvectors$(PS)rn32fm.o) > nul && echo "\033[92m"rn32fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rn32fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv32fm.o ..$(PS)testvectors$(PS)rv32fm.o) > nul && echo "\033[92m"rv32fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rv32fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn32fm.i ..$(PS)testvectors$(PS)rn32fm.i) > nul && echo "\033[92m"rn32fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rn32fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv32fm.i ..$(PS)testvectors$(PS)rv32fm.i) > nul && echo "\033[92m"rv32fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rv32fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri32fa.o ..$(PS)testvectors$(PS)ri32fa.o) > nul && echo "\033[92m"ri32fa.o - BITWISE OK"\033[0m" || echo "\033[91m"ri32fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri32fm.o ..$(PS)testvectors$(PS)ri32fm.o) > nul && echo "\033[92m"ri32fm.o - BITWISE OK"\033[0m" || echo "\033[91m"ri32fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn40fa.i ..$(PS)testvectors$(PS)rn40fa.i) > nul && echo "\033[92m"rn40fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rn40fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv40fa.i ..$(PS)testvectors$(PS)rv40fa.i) > nul && echo "\033[92m"rv40fa.i - BITWISE OK"\033[0m" || echo "\033[91m"rv40fa.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn40fa.o ..$(PS)testvectors$(PS)rn40fa.o) > nul && echo "\033[92m"rn40fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rn40fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv40fa.o ..$(PS)testvectors$(PS)rv40fa.o) > nul && echo "\033[92m"rv40fa.o - BITWISE OK"\033[0m" || echo "\033[91m"rv40fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn40fx.o ..$(PS)testvectors$(PS)rn40fx.o) > nul && echo "\033[92m"rn40fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rn40fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv40fx.o ..$(PS)testvectors$(PS)rv40fx.o) > nul && echo "\033[92m"rv40fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rv40fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn40fx.o ..$(PS)testvectors$(PS)rn40fx.o) > nul && echo "\033[92m"rn40fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rn40fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv40fx.o ..$(PS)testvectors$(PS)rv40fx.o) > nul && echo "\033[92m"rv40fx.o - BITWISE OK"\033[0m" || echo "\033[91m"rv40fx.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn40fm.o ..$(PS)testvectors$(PS)rn40fm.o) > nul && echo "\033[92m"rn40fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rn40fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv40fm.o ..$(PS)testvectors$(PS)rv40fm.o) > nul && echo "\033[92m"rv40fm.o - BITWISE OK"\033[0m" || echo "\033[91m"rv40fm.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rn40fm.i ..$(PS)testvectors$(PS)rn40fm.i) > nul && echo "\033[92m"rn40fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rn40fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)rv40fm.i ..$(PS)testvectors$(PS)rv40fm.i) > nul && echo "\033[92m"rv40fm.i - BITWISE OK"\033[0m" || echo "\033[91m"rv40fm.i - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri40fa.o ..$(PS)testvectors$(PS)ri40fa.o) > nul && echo "\033[92m"ri40fa.o - BITWISE OK"\033[0m" || echo "\033[91m"ri40fa.o - BITWISE BROKEN"\033[0m"
	-@$(BIN_CMP) $(call fix_platform_path, ..$(PS)testvectors$(PS)ref$(PS)ri40fm.o ..$(PS)testvectors$(PS)ri40fm.o) > nul && echo "\033[92m"ri40fm.o - BITWISE OK"\033[0m" || echo "\033[91m"ri40fm.o - BITWISE BROKEN"\033[0m"
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
