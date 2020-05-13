#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

include ..$(PS)..$(PS)rules$(PS)common_build_config.mk

override CFLAGS += -fvectorize -Hloop_sms -Hinline_threshold=555 -Hunswitch=30 -Hunroll=150 -Hpipeline
override ASMFLAGS += 
override LRFLAGS  += 
override LDFLAGS  += 
