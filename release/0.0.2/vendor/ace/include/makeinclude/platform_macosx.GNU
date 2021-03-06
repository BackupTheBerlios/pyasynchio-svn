# platform_macosx.GNU,v 4.18 2004/03/03 00:17:37 dhinton Exp
# platform_macosx.GNU
# support for Mac OS X 10.2

ifndef exceptions
  exceptions = 1
endif
ifeq (,$(debug))
  debug = 1
endif
ifeq (,$(optimize))
  optimize = 0
endif
ifeq (,$(versioned_so))
  versioned_so = 0
endif
with_ld = macosx

pipes ?= 1

CC              = gcc
CXX             = g++
CFLAGS          += -Wall -Wpointer-arith -Wno-long-double -I/sw/include

DCFLAGS         += -g
DLD             = libtool
LD              = $(CXX)
LDFLAGS		+= -L/sw/lib -flat_namespace -undefined warning
LIBS            += -lcc_dynamic -lstdc++ -lSystem

## dlcompat package (not part of base Darwin) is needed for dlopen().
## Fink installer puts libraries in /sw/lib and headers in /sw/include
## In order to install dlcompat do the following:
##   - download fink from http://fink.sf.net
##   - type:
##        fink install dlcompat
LIBS            += -ldl 
OCFLAGS         += -O3
RANLIB          = ranlib
SOEXT		= dylib
SOFLAGS += -dynamic
SOBUILD = -o $(VSHDIR)$*.dylib $<

# Test for template instantiation, add to SOFLAGS if versioned_so set, 
# add -E to LDFLAGS if using GNU ld
#
include $(ACE_ROOT)/include/makeinclude/platform_g++_common.GNU

CCFLAGS         += $(CFLAGS) $(TEMPLATES_FLAG)
