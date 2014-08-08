PROJECT_NAME            = ListAncillaryDataFeatures

PROJECT_DIR             = ../..
EXAMPLES_DIR            = $(PROJECT_DIR)/..
VIMBASDK_DIR			= $(EXAMPLES_DIR)/../..
MAKE_INCLUDE_DIR        = $(CURDIR)/$(EXAMPLES_DIR)/Build/Make

include $(MAKE_INCLUDE_DIR)/Common.mk

CONFIG_DIR          = $(ARCH)_$(WORDSIZE)bit
BIN_FILE            = $(PROJECT_NAME)
BIN_DIR             = binary/$(CONFIG_DIR)
OBJ_DIR             = object/$(CONFIG_DIR)
BIN_PATH            = $(BIN_DIR)/$(BIN_FILE)

all: $(BIN_PATH)

include $(MAKE_INCLUDE_DIR)/VimbaC.mk

SOURCE_DIR          = $(PROJECT_DIR)/Source
COMMON_DIR          = $(EXAMPLES_DIR)/Common

INCLUDE_DIRS        = -I$(SOURCE_DIR) \
                      -I$(EXAMPLES_DIR) \

LIBS                = $(VIMBAC_LIBS)

DEFINES             =

CFLAGS              = $(COMMON_CFLAGS) \
                      $(VIMBAC_CFLAGS)

OBJ_FILES           = $(OBJ_DIR)/ListAncillaryDataFeatures.o \
                      $(OBJ_DIR)/program.o \
                      $(OBJ_DIR)/PrintVimbaVersion.o \
                      $(OBJ_DIR)/ErrorCodeToMessage.o \
                      $(OBJ_DIR)/DiscoverGigECameras.o

DEPENDENCIES        = VimbaC

$(OBJ_DIR)/%.o: $(COMMON_DIR)/%.c $(OBJ_DIR)
	$(CXX) -c $(INCLUDE_DIRS) $(DEFINES) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.c $(OBJ_DIR)
	$(CXX) -c $(INCLUDE_DIRS) $(DEFINES) $(CFLAGS) -o $@ $<

$(BIN_PATH): $(DEPENDENCIES) $(OBJ_FILES) $(BIN_DIR)
	$(CXX) $(ARCH_CFLAGS) -o $(BIN_PATH) $(OBJ_FILES) $(LIBS) -Wl,-rpath,'$$ORIGIN'

clean:
	$(RM) binary -r -f
	$(RM) object -r -f

$(OBJ_DIR):
	$(MKDIR) -p $(OBJ_DIR)

$(BIN_DIR):
	$(MKDIR) -p $(BIN_DIR)