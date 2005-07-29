// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_langinfo.h
 *
 *  language information constants
 *
 *  os_langinfo.h,v 1.3 2004/03/06 05:06:22 bala Exp
 *
 *  @author Don Hinton <dhinton@dresystem.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef ACE_OS_INCLUDE_OS_LANGINFO_H
#define ACE_OS_INCLUDE_OS_LANGINFO_H

#include /**/ "ace/pre.h"

#include "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "ace/os_include/os_nl_types.h"

#if !defined (ACE_LACKS_LANGINFO_H)
# include /**/ <langinfo.h>
#endif /* !ACE_LACKS_LANGINFO_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include /**/ "ace/post.h"
#endif /* ACE_OS_INCLUDE_OS_LANGINFO_H */
