#Possible word sizes: 32, 64
WORDSIZE        = 32

ifneq ($(WORDSIZE),32)
ifneq ($(WORDSIZE),64)
$(error Invalid word size specified)
endif
endif

#Common tools
PKGCFG          = pkg-config
MKDIR           = mkdir
RM              = rm
CXX             = g++
MAKE            = make
CP              = cp

#Debug compile flags
#COMMON_CFLAGS   = -O0 -g

#Release compile flags
COMMON_CFLAGS   = -O3

VIMBASDK_DIR    = $(EXAMPLES_DIR)/../..