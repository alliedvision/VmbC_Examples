include $(MAKE_INCLUDE_DIR)/Common.mk

#Compile options needed for AVTImageTransform
AVTIMAGETRANSFORM_CFLAGS	= -I$(VIMBASDK_DIR)/AVTImageTransform/Include

#Compile options needed for AVTImageTransform
AVTIMAGETRANSFORM_LIBS		= -L$(BIN_DIR) -lAVTImageTransform -Wl,-rpath-link,$(BIN_DIR)

#By default we copy libAVTImageTransform.so next to the binary
$(BIN_DIR)/libAVTImageTransform.so: $(BIN_DIR)
	$(CP) $(VIMBASDK_DIR)/AVTImageTransform/DynamicLib/$(ARCH)_$(WORDSIZE)bit/libAVTImageTransform.so $(BIN_DIR)/

#Operations we have to do in order to prepare AVTImageTransform
AVTImageTransform: $(BIN_DIR)/libAVTImageTransform.so
