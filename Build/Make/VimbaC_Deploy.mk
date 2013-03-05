include $(MAKE_INCLUDE_DIR)/Common.mk

VIMBAC_CFLAGS   = -I$(VIMBASDK_DIR)
VIMBAC_LIBS     = -L$(BIN_DIR) -lVimbaC

$(BIN_DIR)/libVimbaC.so: $(BIN_DIR)
	$(CP) $(VIMBASDK_DIR)/VimbaC/DynamicLib/x86_$(WORDSIZE)bit/libVimbaC.so $(BIN_DIR)/
	
VimbaC: $(BIN_DIR)/libVimbaC.so
