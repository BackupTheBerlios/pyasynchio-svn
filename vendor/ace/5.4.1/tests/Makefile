#----------------------------------------------------------------------------
#
# Makefile,v 4.242 2004/01/07 22:40:16 shuston Exp
#
#       Makefile for the ACE "one-button" tests directory
#----------------------------------------------------------------------------

# This default rule is here so invoking make realclean will do
# work when recursing through subdirectories
.DEFAULT:
	@$(MAKE) -f Makefile.libs $@
	@$(MAKE) -f Makefile.tests $@
	@$(MAKE) -f Makefile.dirs SUBDIR_MAKEFILE=Makefile $@

# Visual Age C++ needs to run strictly sequentially to avoid squashing
# the tempinc directory on parallel makes.
ifdef TEMPINCDIR
.NOTPARALLEL:
endif

# Invoke Makefile.libs first to build the libTest_Output and the other
# libraries, then invoke Makefile.tests and Makefile.dirs to recurse
# through subdirectories
all:
	@$(MAKE) -f Makefile.libs $@
	@$(MAKE) -f Makefile.tests $@
	@$(MAKE) -f Makefile.dirs SUBDIR_MAKEFILE=Makefile $@

