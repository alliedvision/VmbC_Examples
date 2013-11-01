#Operating system
OS				= $(shell uname -s)

PROJECT_ROOT_DIR		= ../..
MAKE_INCLUDE_DIR		= $(CURDIR)/$(PROJECT_ROOT_DIR)/../Externals/GIK/Externals/LLK/Externals/Makefiles

include $(MAKE_INCLUDE_DIR)/$(OS).mk

PROJECT_DIR			= $(OS)_$(ARCH)_$(WORDSIZE)bit/$(CONFIG)
BIN_DIR				= binary/$(PROJECT_DIR)

VIMBAC_FILE			= $(DYNAMIC_PREFIX)VimbaC$(DYNAMIC_SUFFIX)
VIMBAC_BIN			= $(BIN_DIR)/$(VIMBAC_FILE)

VIMBACPP_FILE			= $(DYNAMIC_PREFIX)VimbaCPP$(DYNAMIC_SUFFIX)
VIMBACPP_BIN			= $(BIN_DIR)/$(VIMBACPP_FILE)

EXAMPLES			= AsynchronousGrab \
				  ForceIP \
				  ListCameras \
				  ListFeatures \
				  SynchronousGrab

$(PROJECT_ROOT_DIR)/%/Build/Make/$(BIN_DIR)/%: $(PROJECT_ROOT_DIR)/%/Build/Make/Makefile
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/$*/Build/Make)

$(BIN_DIR)/%: $(PROJECT_ROOT_DIR)/%/Build/Make/$(BIN_DIR)/% $(BIN_DIR)
	$(call MACRO_CP,$(PROJECT_ROOT_DIR)/$*/Build/Make/$(BIN_DIR)/*,$(BIN_DIR))

$(PROJECT_ROOT_DIR)/%/Qt/Build/Make/$(BIN_DIR)/%: $(PROJECT_ROOT_DIR)/%/Qt/Build/Make/Makefile
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/$*/Qt/Build/Make)

$(BIN_DIR)/%: $(PROJECT_ROOT_DIR)/%/Qt/Build/Make/$(BIN_DIR)/% $(BIN_DIR)
	$(call MACRO_CP,$(PROJECT_ROOT_DIR)/$*/Qt/Build/Make/$(BIN_DIR)/*,$(BIN_DIR))

all: $(foreach example,$(EXAMPLES),$(BIN_DIR)/$(example))

clean:
	$(call MACRO_RM,$(BIN_DIR)/*)
	$(call MACRO_RMDIR,$(BIN_DIR))
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/AsynchronousGrab/Build/Make,clean)
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/ForceIP/Build/Make,clean)
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/ListCameras/Build/Make,clean)
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/ListFeatures/Build/Make,clean)
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/SynchronousGrab/Build/Make,clean)

$(BIN_DIR):
	$(call MACRO_MKDIR,$(BIN_DIR))