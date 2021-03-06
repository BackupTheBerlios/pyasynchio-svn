// ICMP_Socket.cpp,v 1.3 2004/09/26 14:12:19 jwillemsen Exp

#include "ace/ICMP_Socket.h"

#if defined (ACE_HAS_ICMP_SUPPORT) && (ACE_HAS_ICMP_SUPPORT == 1)

#include "ace/ACE.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_netdb.h"

#if !defined (__ACE_INLINE__)
# include "ace/ICMP_Socket.inl"
#endif  /* !__ACE_INLINE__ */


ACE_RCSID (ace,
           ICMP_Socket,
           "ICMP_Socket.cpp,v 1.3 2004/09/26 14:12:19 jwillemsen Exp")


namespace ACE
{
  ACE_ALLOC_HOOK_DEFINE (ICMP_Socket)
}


void
ACE::ICMP_Socket::dump (void) const
{
  ACE_TRACE ("ACE::ICMP_Socket::dump");
}

ACE::ICMP_Socket::ICMP_Socket (void)
{
  ACE_TRACE ("ACE::ICMP_Socket::ICMP_Socket");
}

ssize_t
ACE::ICMP_Socket::send (void const * buf,
                        size_t n,
                        ACE_Addr const & addr,
                        int flags) const
{
  ACE_TRACE ("ACE::ICMP_Socket::send");

  return ACE_OS::sendto (this->get_handle (),
                         (char const *) buf,
                         n,
                         flags,
                         (sockaddr const *) addr.get_addr (),
                         addr.get_size ());
}

ssize_t
ACE::ICMP_Socket::recv (void * buf,
                        size_t n,
                        ACE_Addr & addr,
                        int flags) const
{
  ACE_TRACE ("ACE::ICMP_Socket::recv");

  int addr_len = addr.get_size ();
  ssize_t status = ACE_OS::recvfrom (this->get_handle (),
                                     (char *) buf,
                                     n,
                                     flags,
                                     (sockaddr *) addr.get_addr (),
                                     (int*) &addr_len);
  addr.set_size (addr_len);

  return status;
}

ssize_t
ACE::ICMP_Socket::recv (void * buf,
                        size_t n,
                        int flags,
                        ACE_Time_Value const * timeout) const
{
  ACE_TRACE ("ACE::ICMP_Socket::recv");

  return ACE::recv (this->get_handle (),
                    buf,
                    n,
                    flags,
                    timeout);
}

int
ACE::ICMP_Socket::open (ACE_Addr const & local,
                        int protocol,
                        int reuse_addr)
{
  ACE_TRACE ("ACE::ICMP_Socket::open");

  if (! this->check_root_euid ())
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "%p\n", "(%P|%t) ACE::ICMP_Socket::open - "
                         "root-privileges required."),
                        -1);
    }

  //+ if icmp protocol is supported on this host
  int proto_number = -1;
  protoent *proto;

  if (! (proto = getprotobyname ("icmp")))
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "%p\n", "(%P|%t) ACE::ICMP_Socket::open - "
                         "ICMP protocol is not properly configured "
                         "or not supported."),
                        -1);
    }
  proto_number = proto->p_proto;

  if (proto_number != IPPROTO_ICMP)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "%p\n", "(%P|%t) ACE::ICMP_Socket::open - "
                         "only IPPROTO_ICMP protocol is "
                         "currently supported."),
                        -1);
    }

  if (ACE_SOCK::open (SOCK_RAW,
                      AF_INET,
                      protocol,
                      reuse_addr) == -1)
    {
      return -1;
    }

  return this->shared_open (local);
}

int
ACE::ICMP_Socket::shared_open (ACE_Addr const & local)
{
  ACE_TRACE ("ACE::ICMP_Socket::shared_open");

  int error = 0;
  if (local == ACE_Addr::sap_any)
    {
      if (ACE::bind_port (this->get_handle ()) == -1)
        {
          error = 1;
        }
    }
  else if (ACE_OS::bind (this->get_handle (),
                         reinterpret_cast<sockaddr *> (local.get_addr ()),
                         local.get_size ()) == -1)
    {
      error = 1;
    }

  if (error != 0)
    {
      this->close ();
    }

  return error ? -1 : 0;
}

unsigned short
ACE::ICMP_Socket::calculate_checksum (unsigned short * paddress,
                                      int len)
{
  int nleft = len;
  int sum = 0;
  unsigned short * w = paddress;
  unsigned short answer = 0;
  while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }

  if (nleft == 1)
    {
      *((unsigned char *) &answer) = *((unsigned char *) w);
      sum += answer;
    }

  // add back carry outs from top 16 bits to low 16 bits
  sum = (sum >> 16) + (sum & 0xffff); // add hi 16 to low 16
  sum += (sum >> 16);                 // add carry
  answer = ~sum;                      // truncate to 16 bits

  return (answer);
}

int
ACE::ICMP_Socket::check_root_euid (void)
{
  int euid = 0;

#if ! defined (ACE_WIN32)
  euid = static_cast<int> (::geteuid ());
#endif  /* #if ! defined (ACE_WIN32) */

  return (euid == 0);
}

#endif  /* ACE_HAS_ICMP_SUPPORT == 1 */
