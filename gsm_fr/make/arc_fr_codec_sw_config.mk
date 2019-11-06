#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

#REMAPITU_T <on/off> - enable FXAPI and compiler optimization + integrate MWDT ITU-T library for ITU-T codecs
REMAPITU_T ?= on
#Build with link time optimization <on/off>
LTO_BUILD ?= on

#Set option because ARC platform has a signed arithmetic shift right
override CFLAGS += -DSASR

ifeq ($(REMAPITU_T),on)
override CFLAGS   += -DREMAPITU_T
endif

override CFLAGS += 
override ASMFLAGS += 
override LRFLAGS  += 
override LDFLAGS  += 

include ..$(PS)..$(PS)rules$(PS)common_build_config.mk