#Operating system
OS				= $(shell uname -s)
PROJECT_NAME			= AsynchronousGrab
OUTPUT				= binary

PROJECT_ROOT_DIR		= ../..
MAKE_INCLUDE_DIR		= $(CURDIR)/$(PROJECT_ROOT_DIR)/../../Externals/GIK/Externals/LLK/Externals/Makefiles

include $(MAKE_INCLUDE_DIR)/$(OS).mk

PROJECT_DIR			= $(OS)_$(ARCH)_$(WORDSIZE)bit/$(CONFIG)
BIN_FILE			= $($(call MACRO_TOUPPER,$(OUTPUT))_PREFIX)$(PROJECT_NAME)$($(call MACRO_TOUPPER,$(OUTPUT))_SUFFIX)
BIN_DIR				= $(OUTPUT)/$(PROJECT_DIR)
OBJ_DIR				= object/$(PROJECT_DIR)
BIN_PATH			= $(BIN_DIR)/$(BIN_FILE)

SOURCE_DIR			= $(PROJECT_ROOT_DIR)/Source
COMMON_DIR			= $(PROJECT_ROOT_DIR)/../Common

INCLUDE_DIRS			= $(SOURCE_DIR) \
				  $(PROJECT_ROOT_DIR)/.. \


VIMBAC_FILE			= $(DYNAMIC_PREFIX)VimbaC$(DYNAMIC_SUFFIX)
VIMBAC_BIN			= $(BIN_DIR)/$(VIMBAC_FILE)

AVTIMAGETRANSFORM_FILE		= $(DYNAMIC_PREFIX)VimbaImageTransform$(DYNAMIC_SUFFIX)
AVTIMAGETRANSFORM_BIN		= $(BIN_DIR)/$(AVTIMAGETRANSFORM_FILE)

LIBS				= VimbaC \
				  VimbaImageTransform \
				  rt

LIB_DIRS			= $(BIN_DIR)

DEFINES				=

OBJ_FILES			= $(OBJ_DIR)/AsynchronousGrab.o \
				  $(OBJ_DIR)/program.o \
				  $(OBJ_DIR)/PrintVimbaVersion.o \
				  $(OBJ_DIR)/ErrorCodeToMessage.o \
				  $(OBJ_DIR)/DiscoverGigECameras.o

DEPENDENCIES			= $(VIMBAC_BIN) \
				  $(AVTIMAGETRANSFORM_BIN)
all: $(BIN_PATH)

$(OBJ_DIR)/%.o: $(COMMON_DIR)/%.c $(OBJ_DIR)
	$(call MACRO_CXX,$@,$<,$(INCLUDE_DIRS),$(DEFINES))
$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(OBJ_DIR)
	$(call MACRO_CXX,$@,$<,$(INCLUDE_DIRS),$(DEFINES))

$(BIN_PATH): $(DEPENDENCIES) $(OBJ_FILES) $(BIN_DIR)
	$(call MACRO_LINK_$(call MACRO_TOUPPER,$(OUTPUT)),$(BIN_PATH),$(OBJ_FILES),$(LIBS),$(LIB_DIRS),,,'$$ORIGIN')

clean:
	$(call MACRO_RM,$(OBJ_FILES) $(BIN_PATH) $(BIN_DIR)/$(VIMBAC_FILE))
	$(call MACRO_RMDIR,$(OBJ_DIR) $(BIN_DIR))
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/../../Build/Make,clean)
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/../../Externals/AVTImageTransform/UniTransform/build/Make,clean)

$(OBJ_DIR):
	$(call MACRO_MKDIR,$(OBJ_DIR))

$(BIN_DIR):
	$(call MACRO_MKDIR,$(BIN_DIR))

$(PROJECT_ROOT_DIR)/../../Build/Make/dynamic/$(PROJECT_DIR)/$(VIMBAC_FILE):
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/../../Build/Make)

$(VIMBAC_BIN): $(PROJECT_ROOT_DIR)/../../Build/Make/dynamic/$(PROJECT_DIR)/$(VIMBAC_FILE) $(BIN_DIR)
	$(call MACRO_CP,$(PROJECT_ROOT_DIR)/../../Build/Make/dynamic/$(PROJECT_DIR)/$(VIMBAC_FILE),$(VIMBAC_BIN))
$(PROJECT_ROOT_DIR)/../Externals/AVTImageTransform/UniTransform/build/Make/dynamic/$(PROJECT_DIR)/$(AVTIMAGETRANSFORM_FILE):
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/../../Externals/AVTImageTransform/UniTransform/build/Make)

$(AVTIMAGETRANSFORM_BIN): $(PROJECT_ROOT_DIR)/../Externals/AVTImageTransform/UniTransform/build/Make/dynamic/$(PROJECT_DIR)/$(AVTIMAGETRANSFORM_FILE) $(BIN_DIR)
	$(call MACRO_CP,$(PROJECT_ROOT_DIR)/../../Externals/AVTImageTransform/UniTransform/build/Make/dynamic/$(PROJECT_DIR)/$(AVTIMAGETRANSFORM_FILE),$(AVTIMAGETRANSFORM_BIN))
