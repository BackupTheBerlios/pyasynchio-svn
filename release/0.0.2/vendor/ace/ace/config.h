#if defined (_WIN32)

#  include "config-win32.h"
#undef ACE_HAS_NONSTATIC_OBJECT_MANAGER

#elif defined (linux)

#  include "config-linux.h"

#elif defined (sun)

#  include "config-sunos5.8.h"

#elif defined (__xlC__) || defined (__IBMCPP__)

#  include "config-aix5.1.h" 

#endif
