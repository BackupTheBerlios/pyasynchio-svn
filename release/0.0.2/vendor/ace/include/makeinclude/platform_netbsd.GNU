# platform_netbsd.GNU,v 4.11 2004/08/18 08:08:30 jtc Exp

# platform_netbsd.GNU

versioned_so = 1

ifndef exceptions
  exceptions = 1
endif

ifeq (,$(debug))
  debug = 1
endif

ifeq (,$(optimize))
  optimize = 1
endif

ifeq (,$(threads))
  threads = 1
endif

pipes ?= 1

CC              = gcc
CXX		= g++

CFLAGS          += -W -Wall -Wpointer-arith
DCFLAGS         += -g
LDFLAGS		+= -Wl,-rpath $(ACE_ROOT)/ace
DLD             = $(CXX)
LD		= $(CXX)
LIBS            +=
OCFLAGS         += -O2
PIC             = -fpic
AR              = ar
ARFLAGS         = ruv
RANLIB          = ranlib

SOFLAGS += $(CPPFLAGS) -shared
SOBUILD = $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.so  $<
PRELIB = @echo

ifeq ($(threads),1)
LDFLAGS         += -pthread
CFLAGS          += -D_REENTRANT -DACE_HAS_THREADS
endif # threads

# Test for template instantiation, add to SOFLAGS if versioned_so set, 
# add -E to LDFLAGS if using GNU ld
#
include $(ACE_ROOT)/include/makeinclude/platform_g++_common.GNU

CCFLAGS         += $(CFLAGS) $(TEMPLATES_FLAG)

