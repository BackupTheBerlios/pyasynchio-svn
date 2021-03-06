
// server2.cpp,v 1.4 2003/11/09 20:44:19 dhinton Exp

#include "mmap.h"

#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"

int
main (int, char *[])
{
    ACE_Shared_Memory_MM shm_server (SHM_KEY, sizeof(SharedData) );

    char *shm = (char *) shm_server.malloc ();

    ACE_DEBUG ((LM_INFO, "(%P|%t) Memory Mapped file is at 0x%x\n",
                shm ));

    SharedData * sd = new(shm) SharedData;

    sd->set();
    sd->available(0);

    while ( ! sd->available() )
        ACE_OS::sleep (1);

    sd->show();

    if (shm_server.remove () < 0)
        ACE_ERROR ((LM_ERROR, "%p\n", "remove"));

    return 0;
}
