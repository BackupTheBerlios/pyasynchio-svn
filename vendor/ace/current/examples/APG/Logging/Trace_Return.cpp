// Trace_Return.cpp,v 1.2 2004/01/06 19:31:03 shuston Exp

#if defined (__GNUC__) && (__GNUC__ >= 3 || __GNUC_MINOR__ > 95)
// The DEBUG stuff only works with g++ 2.96 and later.

// Listing 1 code/ch03
#include "Trace.h"

void foo (void);

int ACE_TMAIN (int, ACE_TCHAR *[])
{
  TRACE (ACE_TEXT ("main"));
    
  MY_DEBUG (ACE_TEXT ("Hi Mom\n"));
  foo ();
  MY_DEBUG (ACE_TEXT ("Goodnight\n"));

  TRACE_RETURN (0);
}

void foo (void)
{
  TRACE (ACE_TEXT ("foo"));
  MY_DEBUG (ACE_TEXT ("Howdy Pardner\n"));
  TRACE_RETURN_VOID ();
}
// Listing 1

#else
#include <stdio.h>

int main (int, char *[])
{
  puts ("This example only works on g++ 2.96 and later.\n");
  return 0;
}

#endif /* __GNUC__ */
