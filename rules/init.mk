#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

#################################################
# Toolchain settings

# Tools
CC = ccac
LD = $(CC)
LR = ldac
AS = $(CC)
MD = $(CC)
SZ = sizeac
AR = arac

#################################################
# Platform settings
ifneq ($(ComSpec)$(COMSPEC),)
    O_SYS=Windows
    PS=/
    RM=del /Q
    fix_platform_path = $(subst /,\$(nullstring), $(1))
    MAKE = gmake
    MKDIR = mkdir
    BIN_CMP = fc /b
else
    O_SYS=Unix
    PS=/
    RM=rm -f
    fix_platform_path = $(1)
    MAKE = make
    MKDIR = mkdir -p
    BIN_CMP = diff --binary
endif

# Optimization flags
FLAGS_OPT_SPEED ?= -O3
FLAGS_OPT_SIZE  ?= -Os

# Helper functions
get_file_name = $(basename $(notdir $(1)))
get_file_ext  = $(suffix $(notdir $(1)))

#################################################
# Common configuration options
#  DEBUG_MODE   - debug mode (on/off, default: off)
DEBUG_MODE ?= off
ifeq ($(DEBUG_MODE),on)
    override CFLAGS += -g -Hkeepasm -Hanno -DDEBUG
else
    override CFLAGS += -DNDEBUG
endif

#Check MWDT tools
ifeq ($(METAWARE_ROOT),)
$(error METAWARE_ROOT environment variable should be defined as path to Metaware Tools root directory)
endif

#################################################
# Include external configuration files
CONFIG_DIR ?= .

include ..$(PS)..$(PS)rules$(PS)common_hw_config.mk
include $(CONFIG_DIR)$(PS)$(CODEC_ID_PREFIX)_sw_config.mk

#################################################
# Default target global settings

# Library destination directory
# (resulting library file will be placed into 
#  $BINARY_DIR by default)
BINARY_DIR  ?= ..$(PS)lib

# Build files (temporaty and OBJ-files) directory
BUILD_DIR   ?= ..$(PS)obj
APP_BUILD_DIR=$(BUILD_DIR)$(PS)app