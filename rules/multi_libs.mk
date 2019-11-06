#
# Copyright 2019, Synopsys, Inc.
# All rights reserved.
#
# This source code is licensed under the BSD-3-Clause license found in
# the LICENSE file in the root directory of this source tree.
#

ifeq ($(LTO_BUILD),on)
$(FULL_LIB_NAME_DEC).lib: $(LIB_OBJS_OPT_SIZE) $(LIB_OBJS_OPT_SPEED)
	@echo [LR] [Linking dynamic output library $@] 
	@$(CC) -r $(CFLAGS) $(FLAGS_OPT_SPEED) $(LRFLAGS) -Hmerge_exports_file=$(SYMBOLS_FILE_DEC) -Wl,-J -Wl,$(SYMBOLS_FILE_DEC) -Wl,-m -Wl,-C -Wl,output=$(LIB_NAME)_dec_lib.map $^ -o $@ 

$(FULL_LIB_NAME_ENC).lib: $(LIB_OBJS_OPT_SIZE) $(LIB_OBJS_OPT_SPEED)
	@echo [LR] [Linking dynamic output library $@] 
	@$(CC) -r $(CFLAGS) $(FLAGS_OPT_SPEED) $(LRFLAGS) -Hmerge_exports_file=$(SYMBOLS_FILE_ENC) -Wl,-J -Wl,$(SYMBOLS_FILE_ENC) -Wl,-m -Wl,-C -Wl,output=$(LIB_NAME)_enc_lib.map $^ -o $@ 

else
$(FULL_LIB_NAME_DEC).lib: $(LIB_OBJS_OPT_SIZE) $(LIB_OBJS_OPT_SPEED)
	@echo [LR] [Linking dynamic output library $@] 
	@$(CC) -r $(CFLAGS) $(LRFLAGS) -Wl,-J -Wl,$(SYMBOLS_FILE_DEC) -o $@ $^


$(FULL_LIB_NAME_ENC).lib: $(LIB_OBJS_OPT_SIZE) $(LIB_OBJS_OPT_SPEED)
	@echo [LR] [Linking dynamic output library $@] 
	@$(CC) -r $(CFLAGS) $(LRFLAGS) -Wl,-J -Wl,$(SYMBOLS_FILE_ENC) -o $@ $^
endif

$(ARCHIVES): $(LIB_OBJS_OPT_SIZE) $(LIB_OBJS_OPT_SPEED)
	@echo [LD] [Archiving static output library $@] 
	@$(AR) -cq $@ $^