# Makefile,v 1.1 2004/01/01 21:01:00 shuston Exp
#
# Copyright 2003 Addison-Wesley Inc. All Rights Reserved.
# Makefile for Reactor-based examples

BIN =	Client \
	HAStatus \
	HAStatus-AC \
	Reschedule \
	Schedule_Timers \
	Timer_Cancel \
	Timers \
	Timer_State_Data

include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include $(ACE_ROOT)/include/makeinclude/macros.GNU
include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
include $(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

# To build multiple executables in the same directory on AIX, it works
# best to wipe out any previously-created tempinc directory.
# The compiler/linker isn't too smart about instantiating templates...
ifdef TEMPINCDIR
COMPILE.cc := $(RM) -rf tempinc; $(COMPILE.cc)
endif
