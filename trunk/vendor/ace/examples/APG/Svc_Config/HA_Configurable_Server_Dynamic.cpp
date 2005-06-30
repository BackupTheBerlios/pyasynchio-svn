// HA_Configurable_Server_Dynamic.cpp,v 1.1 2004/01/01 21:01:00 shuston Exp

// Listing 1 code/ch19
#include "ace/OS.h"
#include "ace/Service_Config.h"
#include "ace/Reactor.h"

int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  ACE_Service_Config::open (argc, argv);
  ACE_Reactor::instance ()->run_reactor_event_loop ();
  return 0;
}
// Listing 1
