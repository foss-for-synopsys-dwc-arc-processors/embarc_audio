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

.PHONY: all run clean cleanall 

APP_NAME = g711demo.elf

# Setting target
all: $(APP_BUILD_DIR) $(APP_NAME)  

CLEAN_BINS += $(APP_NAME)
MAP_NAME   += g711demo.map


APP_C_FILES = g711demo.c

include ..$(PS)..$(PS)rules$(PS)common_app.mk
##################################################
# Run application

ifneq (,$(TCF_FLAG))

TEST_VECTOR ?= sweep

run: run_a_encoder run_a_decoder run_a_codec run_mu_encoder run_mu_decoder run_mu_codec

run_a_encoder: all
	@echo Running test vector $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) A lilo ..$(PS)testvectors$(PS)inp$(PS)$(TEST_VECTOR).src ..$(PS)testvectors$(PS)$(TEST_VECTOR).a 256 1 256
	$(BIN_CMP) $(call fix_platform_path, ..\testvectors\ref\$(TEST_VECTOR)-r.a ..\testvectors\$(TEST_VECTOR).a)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)$(TEST_VECTOR).a)

run_a_decoder: all
	@echo Running test vector $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) A loli ..$(PS)testvectors$(PS)inp$(PS)$(TEST_VECTOR)-r.a ..$(PS)testvectors$(PS)$(TEST_VECTOR).rea 256 1 256
	$(BIN_CMP) $(call fix_platform_path, ..\testvectors\ref\$(TEST_VECTOR)-r.rea ..\testvectors\$(TEST_VECTOR).rea)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)$(TEST_VECTOR).rea)

run_a_codec: all
	@echo Running test vector $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) A lili ..$(PS)testvectors$(PS)inp$(PS)$(TEST_VECTOR).src ..$(PS)testvectors$(PS)$(TEST_VECTOR).a-a 256 1 256
	$(BIN_CMP) $(call fix_platform_path, ..\testvectors\ref\$(TEST_VECTOR)-r.a-a ..\testvectors\$(TEST_VECTOR).a-a)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)$(TEST_VECTOR).a-a)

run_mu_encoder: all
	@echo Running test vector $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) u lilo ..$(PS)testvectors$(PS)inp$(PS)$(TEST_VECTOR).src ..$(PS)testvectors$(PS)$(TEST_VECTOR).u 256 1 256
	$(BIN_CMP) $(call fix_platform_path, ..\testvectors\ref\$(TEST_VECTOR)-r.u ..\testvectors\$(TEST_VECTOR).u)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)$(TEST_VECTOR).u)

run_mu_decoder: all
	@echo Running test vector $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) u loli ..$(PS)testvectors$(PS)inp$(PS)$(TEST_VECTOR)-r.u ..$(PS)testvectors$(PS)$(TEST_VECTOR).reu 256 1 256
	$(BIN_CMP) $(call fix_platform_path, ..\testvectors\ref\$(TEST_VECTOR)-r.reu ..\testvectors\$(TEST_VECTOR).reu)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)$(TEST_VECTOR).reu)

run_mu_codec: all
	@echo Running test vector $(TEST_VECTOR) ...
	mdb $(DBG_HWFLAGS) $(APP_NAME) u lili ..$(PS)testvectors$(PS)inp$(PS)$(TEST_VECTOR).src ..$(PS)testvectors$(PS)$(TEST_VECTOR).u-u 256 1 256
	$(BIN_CMP) $(call fix_platform_path, ..\testvectors\ref\$(TEST_VECTOR)-r.u-u ..\testvectors\$(TEST_VECTOR).u-u)
	@$(RM) $(call fix_platform_path, ..$(PS)testvectors$(PS)$(TEST_VECTOR).u-u)

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
