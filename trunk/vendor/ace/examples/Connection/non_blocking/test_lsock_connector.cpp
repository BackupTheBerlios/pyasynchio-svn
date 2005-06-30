// test_lsock_connector.cpp,v 4.12 2003/07/11 05:14:36 irfan Exp

#include "ace/LSOCK_Connector.h"
#include "ace/Log_Msg.h"

#if defined (ACE_LACKS_UNIX_DOMAIN_SOCKETS)
int
main (int, char *argv[])
{
  ACE_ERROR_RETURN ((LM_INFO,
                     ACE_TEXT ("%s: not supported with ")
                     ACE_TEXT ("ACE_LACKS_UNIX_DOMAIN_SOCKETS\n"),
                     argv[0]),
                     -1);
}

#else  /* ! ACE_LACKS_UNIX_DOMAIN_SOCKETS */

#include "ace/UNIX_Addr.h"
#include "CPP-connector.h"

ACE_RCSID(non_blocking, test_lsock_connector, "test_lsock_connector.cpp,v 4.12 2003/07/11 05:14:36 irfan Exp")

typedef Peer_Handler<ACE_LSOCK_STREAM> PEER_HANDLER;
typedef IPC_Client<PEER_HANDLER, ACE_LSOCK_CONNECTOR> IPC_CLIENT;

// ACE_LSOCK Client.

int
main (int argc, char *argv[])
{
  // Perform Service_Config initializations
  ACE_Service_Config daemon (argv[0]);

  IPC_CLIENT peer_connector;

  if (peer_connector.init (argc, argv) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "init"),
                      -1);

  return peer_connector.svc ();
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Connector_Base<PEER_HANDLER>;
template class ACE_Connector<PEER_HANDLER, ACE_LSOCK_CONNECTOR>;
template class ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_SYNCH>;
template class ACE_NonBlocking_Connect_Handler<PEER_HANDLER>;
template class IPC_Client<PEER_HANDLER, ACE_LSOCK_CONNECTOR>;
template class Peer_Handler<ACE_LSOCK_STREAM>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Connector_Base<PEER_HANDLER>
#pragma instantiate ACE_Connector<PEER_HANDLER, ACE_LSOCK_CONNECTOR>
#pragma instantiate ACE_Svc_Handler<ACE_LSOCK_STREAM, ACE_SYNCH>
#pragma instantiate ACE_NonBlocking_Connect_Handler<PEER_HANDLER>
#pragma instantiate IPC_Client<PEER_HANDLER, ACE_LSOCK_CONNECTOR>
#pragma instantiate Peer_Handler<ACE_LSOCK_STREAM>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

#endif /* ! ACE_LACKS_UNIX_DOMAIN_SOCKETS */
