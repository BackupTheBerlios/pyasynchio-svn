// -*- C++ -*-

//=============================================================================
/**
 *  @file    Ping_Socket.h
 *
 *  Ping_Socket.h,v 1.2 2004/09/26 10:09:40 jwillemsen Exp
 *
 *  @author Robert S. Iakobashvili <roberti@go-WLAN.com> <coroberti@walla.co.il>
 *  @author Gonzalo A. Diethelm <gonzalo.diethelm@aditiva.com> made aceing
 */
//=============================================================================

#ifndef ACE_PING_SOCKET_H
#define ACE_PING_SOCKET_H

#include /**/ "ace/pre.h"

#include "ace/ACE_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined (ACE_HAS_ICMP_SUPPORT) && (ACE_HAS_ICMP_SUPPORT == 1)

#include "ace/ICMP_Socket.h"

class ACE_INET_Addr;

namespace ACE
{
  /**
   * @class ACE_Ping_Socket
   *
   * @brief This class is useful to perform ICMP echo checks (pinging)
   * on the party of your interest.  It may be used as well to check
   * LAN-adapters against 3-rd parties.
   */
  class ACE_Export Ping_Socket : public ICMP_Socket
  {
    typedef ICMP_Socket inherited;

  public:

    // = Initialization and termination methods.

    /// Default constructor.
    Ping_Socket (void);

    Ping_Socket (ACE_Addr const & local,
                 int protocol    = IPPROTO_ICMP,
                 int reuse_addr  = 0);

    /// Destructor.
    ~Ping_Socket (void);

    /// Wrapper around the BSD-style @c socket system call (no QoS).
    int open (ACE_Addr const & local = ACE_Addr::sap_any,
              int protocol = IPPROTO_ICMP,
              int reuse_addr = 0);

    /// @a toConnect = 1 - makes connect to remote address
    int send_echo_check (ACE_INET_Addr & remote_addr,
                         int to_connect = 0);

    /// To receive @c ICMP_ECHOREPLY. To be called after successfully
    /// sending @c ICMP_ECHO.
    int process_incoming_dgram (char * ptr, ssize_t len);

    /// @a toConnect = 1 - makes connect to remote address
    int make_echo_check (ACE_INET_Addr & remote_addr,
                         int to_connect = 0,
                         ACE_Time_Value const * timeout = &time_default_);

    char * icmp_recv_buff (void);

    /// Dump the state of an object.
    void dump (void) const;

    /// Declare the dynamic allocation hooks.
    ACE_ALLOC_HOOK_DECLARE;

  public:

    enum
      {
        PING_BUFFER_SIZE = (1024 * 2),
      };

    static ACE_Time_Value const time_default_;

  private:

    int receive_echo_reply (ACE_Time_Value const * timeout);

    /// Do not allow this function to percolate up to this interface.
    int get_remote_addr (ACE_INET_Addr &addr) const;

    char icmp_send_buff_[PING_BUFFER_SIZE];

    char icmp_recv_buff_[PING_BUFFER_SIZE];

    unsigned long sequence_number_;

    int connected_socket_;

  };

} // End namespace ACE


#if defined (__ACE_INLINE__)
# include "ace/Ping_Socket.inl"
#endif /* __ACE_INLINE__ */

#endif  /* ACE_HAS_ICMP_SUPPORT == 1 */

#include /**/ "ace/post.h"

#endif /* ACE_PING_SOCKET_H */
