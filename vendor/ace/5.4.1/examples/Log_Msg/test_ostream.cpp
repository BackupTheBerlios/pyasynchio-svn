// test_ostream.cpp,v 4.11 2003/11/10 01:48:03 dhinton Exp

// ============================================================================
//
// = LIBRARY
//    examples/Log_Msg
//
// = FILENAME
//    test_ostream.cpp
//
// = DESCRIPTION
//     This program tests the Log_Msg abstraction wrt writing to
//     stderr and to a file.
//
// = AUTHOR
//    Irfan Pyarali
//
// ============================================================================

#include "ace/OS_main.h"

// FUZZ: disable check_for_streams_include
#include "ace/streams.h"

#include "ace/Log_Msg.h"

ACE_RCSID(Log_Msg, test_ostream, "test_ostream.cpp,v 4.11 2003/11/10 01:48:03 dhinton Exp")

int
ACE_TMAIN (int, ACE_TCHAR *[])
{
  // This message should show up in stderr.
  ACE_DEBUG ((LM_DEBUG,
              "first message\n"));

  ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);

  // This message should not show up anywhere.
  ACE_DEBUG ((LM_DEBUG,
              "second message\n"));

  ACE_LOG_MSG->set_flags (ACE_Log_Msg::OSTREAM);

  // This message should not show up anywhere since no ostream has
  // been specified.
  ACE_DEBUG ((LM_DEBUG,
              "third message\n"));

#if !defined (ACE_LACKS_IOSTREAM_TOTALLY)
  // Create a persistent store.
  const char *filename = "output";
  ofstream myostream (filename, ios::out | ios::trunc);

  // Check for errors.
  if (myostream.bad ())
    return 1;

  // Set the ostream.
  ACE_LOG_MSG->msg_ostream (&myostream);

  // This message should show up in the ostream.
  ACE_DEBUG ((LM_DEBUG,
              "fourth message\n"));
#endif /* ACE_LACKS_IOSTREAM_TOTALLY */

  ACE_LOG_MSG->set_flags (ACE_Log_Msg::STDERR);

  // This message should show up in stderr and the ostream (without
  // ACE_LACKS_IOSTREAM_TOTALLY).
  ACE_DEBUG ((LM_DEBUG,
              "fifth message\n"));

  return 0;
}
