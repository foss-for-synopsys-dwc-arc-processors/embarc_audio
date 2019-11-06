#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

DEP_MAKES = Makefile .$(PS)..$(PS)..$(PS)rules$(PS)init.mk ..$(PS)..$(PS)rules$(PS)common_hw_config.mk \
            $(CONFIG_DIR)$(PS)$(CODEC_ID_PREFIX)_sw_config.mk .$(PS)..$(PS)..$(PS)rules$(PS)common.mk \
            .$(PS)..$(PS)..$(PS)rules$(PS)common_app.mk .$(PS)..$(PS)..$(PS)rules$(PS)init.mk \
            .$(PS)..$(PS)..$(PS)rules$(PS)multi_libs.mk .$(PS)..$(PS)..$(PS)rules$(PS)single_lib.mk

ifeq ($(LTO_BUILD),on)
    override LIB_CFLAGS += -Hlto -Hlto_mode=safe 
else
    override LIB_CFLAGS += -Hldopt=-q,-Coutput=$@.map,-Csections
endif

override LIB_CFLAGS += -Hsdata0 -Hnocopyr -Hpurge -MMD -MP -Houtdir=$(BUILD_DIR) -Hnoivt -Hnoxcheck
override LRFLAGS += -q -Wl,-s,-zlistunref -Hheap=0 -Hstack=0 $(LIB_CFLAGS)

ifeq ($(REMAPITU_T),on)
override CFLAGS += -Xdsp_ctrl=noguard,up,preshift -Hitut -Hfxapi
else
ifeq ($(FX_MAPPING),on)
override CFLAGS += -Hfxapi -Xdsp_ctrl=noguard,up,postshift 
else
override CFLAGS += -fno-builtin-round
endif
endif

override LIB_INCLUDE_DIRS_COMMON += ..$(PS)include \
                                    ..$(PS)src$(PS)include \
                                    ..$(PS)..$(PS)testlib$(PS)include

override LIB_C_FILES_DIRS_COMMON += ..$(PS)src

override LIB_INCLUDE_DIRS += $(LIB_INCLUDE_DIRS_COMMON)
LIB_C_FILES_DIRS = $(LIB_C_FILES_DIRS_COMMON)
LIB_C_FILES_OPT_SIZE = $(LIB_C_FILES_OPT_SIZE_COMMON)
LIB_C_FILES_OPT_SPEED = $(LIB_C_FILES_OPT_SPEED_COMMON)

# Internal definitions (do not modify)
LIB_OBJS_C_OPT_SIZE   	= $(addprefix $(BUILD_DIR)$(PS), $(LIB_C_FILES_OPT_SIZE:.c=.o))
LIB_OBJS_C_OPT_SPEED  	= $(addprefix $(BUILD_DIR)$(PS), $(LIB_C_FILES_OPT_SPEED:.c=.o))
LIB_DEPENDS_C_OPT_SIZE   	= $(addprefix $(BUILD_DIR)$(PS), $(LIB_C_FILES_OPT_SIZE:.c=.d))
LIB_DEPENDS_C_OPT_SPEED  	= $(addprefix $(BUILD_DIR)$(PS), $(LIB_C_FILES_OPT_SPEED:.c=.d))

LIB_INCS              	= $(foreach dir,$(LIB_INCLUDE_DIRS), $(addprefix -I,$(dir)))

LIB_OBJS_OPT_SPEED =  $(LIB_OBJS_C_OPT_SPEED)
LIB_OBJS_OPT_SIZE  =  $(LIB_OBJS_C_OPT_SIZE)

CLEAN_OBJS           += $(LIB_OBJS_OPT_SIZE) $(LIB_OBJS_OPT_SPEED)
CLEAN_DEPENDS        += $(LIB_DEPENDS_C_OPT_SIZE) $(LIB_DEPENDS_C_OPT_SPEED)

# Directories creation
$(BUILD_DIR):
	@echo [MKDIR] [Creating directory $@]      
	@$(MKDIR) $(call fix_platform_path, $(BUILD_DIR))

$(BINARY_DIR):
	@echo [MKDIR] [Creating directory $@]      
	@$(MKDIR) $(call fix_platform_path, $(BINARY_DIR))

ifeq ($(COMPONENT),ENCODER)
ARCHIVES = $(FULL_LIB_NAME_ENC).a
else
ifeq ($(COMPONENT),DECODER)
ARCHIVES = $(FULL_LIB_NAME_DEC).a 
else
ARCHIVES = $(FULL_LIB_NAME_DEC).a $(FULL_LIB_NAME_ENC).a
endif
endif

# Library linkage rule
ifeq ($(MULTI_LIBS_MODE),on)
    include ..$(PS)..$(PS)rules$(PS)multi_libs.mk
else
    include ..$(PS)..$(PS)rules$(PS)single_lib.mk
endif #($(CODEC_ID_PREFIX), arc_g722IV_codec)

# Library sources compilation rules
ifneq ($(findstring on,$(LTO_BUILD)),on)
ifeq ($(findstring Hmerge,$(CFLAGS)),Hmerge)
$(LIB_OBJS_OPT_SPEED): $(LIB_C_FILES_OPT_SPEED) $(DEP_MAKES)
	@echo Compiling C and C++ [Opt. SPEED with -Hmerge]: $(CC) $< ...
	@$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SPEED) -Hmerge_exports_file=$(SYMBOLS_FILE) -Houtdir=$(BUILD_DIR) -c $^ -o $@
else
$(LIB_OBJS_C_OPT_SPEED): $(BUILD_DIR)$(PS)%.o: %.c $(DEP_MAKES)
	@echo Compiling C [Opt. SPEED]: $(CC) $< ...
	@$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SPEED) -c $< -o $@

$(LIB_OBJS_C_OPT_SIZE): $(BUILD_DIR)$(PS)%.o: %.c $(DEP_MAKES)
	@echo Compiling C [Opt. SIZE]: $(CC) $< ...
	@$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SIZE) -c $< -o $@
endif
else

$(LIB_OBJS_C_OPT_SPEED): $(BUILD_DIR)$(PS)%.o: %.c $(DEP_MAKES)
	@echo Compiling C [Opt. SPEED]: $(CC) $< ...
	@$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SPEED) -c $< -o $@

$(LIB_OBJS_C_OPT_SIZE): $(BUILD_DIR)$(PS)%.o: %.c $(DEP_MAKES)
	@echo Compiling C [Opt. SIZE]: $(CC) $< ...
	@$(CC) $(LIB_INCS) $(LIB_CFLAGS) $(CFLAGS) $(FLAGS_OPT_SIZE) -c $< -o $@
endif
