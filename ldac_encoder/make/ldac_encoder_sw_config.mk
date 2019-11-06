#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

#FX_MAPPING <on/off> - enable FXAPI and compiler optimization for NOT ITU-T codecs
FX_MAPPING ?= on

ifeq ($(FX_MAPPING),on)
override CFLAGS   += -Haggressive_unroll -Hunroll=150 -Hmisched -Hmax_predicate=0 -Hunswitch=200
endif

override CFLAGS += -D_32BIT_FIXED_POINT
override ASMFLAGS += 
override LRFLAGS  += 
override LDFLAGS  += 

include ..$(PS)..$(PS)rules$(PS)common_build_config.mk
