#----------------------------------------------------------------------------
#	Makefile,v 4.10 2003/04/25 15:20:07 gthaker Exp
#
#	Makefile for the performance-tests directory
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#	Local macros
#----------------------------------------------------------------------------

INFO  = README

DIRS  = Misc \
	UDP \
	TCP \
	SCTP \
	Server_Concurrency

# @@ This directory is currently under restructuring.  It should be
#    added back to the makefile once this is done.
# Synch-Benchmarks \

# The following directory isn't compiled by default since haven't
# finished integrating it into ACE...
#
# TTCP

#----------------------------------------------------------------------------
#	Include macros and targets
#----------------------------------------------------------------------------

include	$(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include	$(ACE_ROOT)/include/makeinclude/macros.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.common.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.nested.GNU
include	$(ACE_ROOT)/include/makeinclude/rules.nolocal.GNU
