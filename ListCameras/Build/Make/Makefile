#Operating system
OS				= $(shell uname -s)
PROJECT_NAME			= ListCameras
PROJECT_TYPE			= BINARY

PROJECT_ROOT_DIR		= ../..
MAKE_INCLUDE_DIR		= $(CURDIR)/$(PROJECT_ROOT_DIR)/../../Externals/GIK/Externals/LLK/Externals/Makefiles

include $(MAKE_INCLUDE_DIR)/$(OS).mk

PROJECT_DIR			= $(OS)_$(ARCH)/$(CONFIG)
BIN_FILE			= $($(PROJECT_TYPE)_PREFIX)$(PROJECT_NAME)$($(PROJECT_TYPE)_SUFFIX)
BIN_DIR				= bin/$(PROJECT_DIR)
OBJ_DIR				= obj/$(PROJECT_DIR)
BIN_PATH			= $(BIN_DIR)/$(BIN_FILE)

SOURCE_DIR			= $(PROJECT_ROOT_DIR)/Source
INCLUDE_DIRS			= $(SOURCE_DIR) \
				  $(PROJECT_ROOT_DIR)/../..

VIMBAC_BIN			= $(PROJECT_ROOT_DIR)/../../Build/Make/$(BIN_DIR)/$(DYNAMIC_PREFIX)VimbaC$(DYNAMIC_SUFFIX)

LIBS				= VimbaC

LIB_DIRS			= $(PROJECT_ROOT_DIR)/../../Build/Make/$(BIN_DIR)

DEFINES				=

OBJ_FILES			= $(OBJ_DIR)/ListCameras.o \
				  $(OBJ_DIR)/program.o

DEPENDENCIES			= $(VIMBAC_BIN)

all: $(BIN_PATH)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(OBJ_DIR)
	$(call MACRO_CXX,$@,$<,$(INCLUDE_DIRS),$(DEFINES))

$(BIN_PATH): $(DEPENDENCIES) $(OBJ_FILES) $(BIN_DIR)
	$(call MACRO_LINK_$(PROJECT_TYPE),$(BIN_PATH),$(OBJ_FILES),$(LIBS),$(LIB_DIRS))

clean:
	$(call MACRO_RM,$(OBJ_FILES) $(BIN_PATH))
	$(call MACRO_RMDIR,$(OBJ_DIR) $(BIN_DIR))
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/../../Build/Make,clean)

$(OBJ_DIR):
	$(call MACRO_MKDIR,$(OBJ_DIR))

$(BIN_DIR):
	$(call MACRO_MKDIR,$(BIN_DIR))

$(VIMBAC_BIN):
	$(call MACRO_MAKE,$(PROJECT_ROOT_DIR)/../../Build/Make)
