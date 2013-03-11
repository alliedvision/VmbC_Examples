UNAME		= $(shell uname -m)

ifeq ($(UNAME),i386)
AUTOWORDSIZE	= 32
endif
ifeq ($(UNAME),i486)
AUTOWORDSIZE	= 32
endif
ifeq ($(UNAME),i586)
AUTOWORDSIZE	= 32
endif
ifeq ($(UNAME),i686)
AUTOWORDSIZE	= 32
endif
ifeq ($(UNAME),x86_64)
AUTOWORDSIZE	= 64
endif
ifeq ($(UNAME),amd64)
AUTOWORDSIZE	= 64
endif

#Possible word sizes: 32, 64
WORDSIZE        = $(AUTOWORDSIZE)

ifneq ($(WORDSIZE),32)
ifneq ($(WORDSIZE),64)
$(error Invalid word size set)
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
#CONFIG_CFLAGS  = -O0 -g

#Release compile flags
CONFIG_CFLAGS   = -O3

COMMON_CFLAGS   = $(CONFIG_CFLAGS) -fPIC