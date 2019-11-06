#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

ifeq ($(LTO_BUILD),on)
$(BINARY_DIR)$(PS)$(LIB_NAME).lib: $(LIB_OBJS_C_OPT_SIZE) $(LIB_OBJS_C_OPT_SPEED)
	@echo [LR] [Linking dynamic output library $@] 
	@$(CC) -r $(CFLAGS) $(FLAGS_OPT_SPEED) $(LRFLAGS) -Hmerge_exports_file=$(SYMBOLS_FILE) -Wl,-J -Wl,$(SYMBOLS_FILE) -Wl,-m -Wl,-C -Wl,output=$(LIB_NAME)_lib.map $^ -o $@ 

else
$(BINARY_DIR)$(PS)$(LIB_NAME).lib: $(LIB_OBJS_C_OPT_SIZE) $(LIB_OBJS_C_OPT_SPEED)
	@echo [LR] [Linking dynamic output library $@] 
	@$(CC) -r $(CFLAGS) $(LRFLAGS) -Wl,-J -Wl,$(SYMBOLS_FILE) -o $@ $^
endif

$(BINARY_DIR)$(PS)$(LIB_NAME).a: $(LIB_OBJS_C_OPT_SIZE) $(LIB_OBJS_C_OPT_SPEED)
	@echo [LD] [Archiving static output library $@] 
	@$(AR) -cq $@ $^