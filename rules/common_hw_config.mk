#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#
 
# Hardware configuration refers to tcf file
# the following lines should be edited manually:
#  - provide path to the tcf file with the required hardware configuration
#    example: override CFLAGS += -tcf=".$(PS)arc.tcf"
TCF ?= em9d_voice_audio.tcf
override CFLAGS := -tcf=$(TCF) -tcf_core_config $(CFLAGS)
