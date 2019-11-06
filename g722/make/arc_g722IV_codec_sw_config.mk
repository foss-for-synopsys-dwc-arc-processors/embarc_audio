#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

#REMAPITU_T <on/off> - enable FXAPI and compiler optimization + integrate MWDT ITU-T library for ITU-T codecs
REMAPITU_T ?= on
#Calculate Weighted MOPS <on/off>
WMOPS ?= off
#Use XCCM for storing tables of coefficients
USE_XCCM ?= on
#Build with link time optimization <on/off>
LTO_BUILD ?= on

ifeq ($(USE_XCCM),on)
override CFLAGS += -DUSE_XCCM
endif

ifeq ($(WMOPS),on)
override CFLAGS += -DWMOPS
REMAPITU_T ?= off
endif

ifeq ($(REMAPITU_T),on)
override CFLAGS   += -DREMAPITU_T
override CFLAGS   +=-Haggressive_unroll -Hinline_threshold=555 -Hloop_sms -Hunroll=150  -Hunswitch=30

ALL_PLATFORM_FLAGS := '$(shell ccac -tcf=$(TCF) -Hbatchnotmp _.c)'
ifneq (,$(filter -av2hs,$(ALL_PLATFORM_FLAGS)))
override CFLAGS +=  -Hpipeline
endif
else
override FLAGS_OPT_SPEED =
override FLAGS_OPT_SIZE =
endif

override CFLAGS += 
override ASMFLAGS += 
override LRFLAGS  += 
override LDFLAGS  += 

include ..$(PS)..$(PS)rules$(PS)common_build_config.mk
