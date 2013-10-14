include $(MAKE_INCLUDE_DIR)/Common.mk

#Compile options needed for VimbaC
VIMBAC_CFLAGS   = -I$(VIMBASDK_DIR)

#Linker options needed for VimbaC
VIMBAC_LIBS     = -L$(BIN_DIR) -lVimbaC

#By default we copy libVimbaC.so next to the binary
$(BIN_DIR)/libVimbaC.so: $(BIN_DIR)
	$(CP) $(VIMBASDK_DIR)/VimbaC/DynamicLib/$(ARCH)_$(WORDSIZE)bit/libVimbaC.so $(BIN_DIR)/
	
#Operations we have to do in order to prepare VimbaC
VimbaC: $(BIN_DIR)/libVimbaC.so
