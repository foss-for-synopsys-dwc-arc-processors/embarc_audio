#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

#Common makefile settings
NATIVE_CYCLE_PROFILING ?= off
NATIVE_STACK_PROFILING ?= off
NATIVE_BUSBANDWIDTH_PROFILING ?= off

#Build with link time optimization <on/off>
LTO_BUILD ?= off

ifeq ($(FX_MAPPING),on)
override CFLAGS +=  -Hon=Long_enums -Hdense_prologue -fslp-vectorize-aggressive
else
ifeq ($(REMAPITU_T),on)
override CFLAGS +=  -Hon=Long_enums -Hdense_prologue -fslp-vectorize-aggressive
endif
endif

ifeq ($(NATIVE_CYCLE_PROFILING),on)
override CFLAGS   += -DNATIVE_CYCLE_PROFILING
endif

ifeq ($(NATIVE_STACK_PROFILING),on)
override CFLAGS   += -DNATIVE_STACK_PROFILING
endif

ifeq ($(NATIVE_BUSBANDWIDTH_PROFILING),on)
override CFLAGS   += -DNATIVE_BUSBANDWIDTH_PROFILING
endif