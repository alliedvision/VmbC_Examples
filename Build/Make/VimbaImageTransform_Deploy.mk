include $(MAKE_INCLUDE_DIR)/Common.mk

#Compile options needed for VimbaImageTransform
VIMBAIMAGETRANSFORM_CFLAGS	= -I$(VIMBASDK_DIR)/VimbaImageTransform/Include

#Compile options needed for VimbaImageTransform
VIMBAIMAGETRANSFORM_LIBS		= -L$(BIN_DIR) -lVimbaImageTransform -Wl,-rpath-link,$(BIN_DIR)

#By default we copy libVimbaImageTransform.so next to the binary
$(BIN_DIR)/libVimbaImageTransform.so: $(BIN_DIR)
	$(CP) $(VIMBASDK_DIR)/VimbaImageTransform/DynamicLib/$(ARCH)_$(WORDSIZE)bit/libVimbaImageTransform.so $(BIN_DIR)/

#Operations we have to do in order to prepare VimbaImageTransform
VimbaImageTransform: $(BIN_DIR)/libVimbaImageTransform.so
