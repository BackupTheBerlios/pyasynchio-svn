/* -*- C++ -*- */
// LSOCK.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp

// Simple-minded constructor.

ACE_INLINE
ACE_LSOCK::ACE_LSOCK (void)
  : aux_handle_ (ACE_INVALID_HANDLE)
{
  ACE_TRACE ("ACE_LSOCK::ACE_LSOCK");
}

// Sets the underlying file descriptor.

ACE_INLINE void
ACE_LSOCK::set_handle (ACE_HANDLE handle)
{
  ACE_TRACE ("ACE_LSOCK::set_handle");
  this->aux_handle_ = handle;
}

// Gets the underlying file descriptor.

ACE_INLINE ACE_HANDLE
ACE_LSOCK::get_handle (void) const
{
  ACE_TRACE ("ACE_LSOCK::get_handle");
  return this->aux_handle_;
}

// Sets the underlying file descriptor.

ACE_INLINE
ACE_LSOCK::ACE_LSOCK (ACE_HANDLE handle)
  : aux_handle_ (handle)
{
  ACE_TRACE ("ACE_LSOCK::ACE_LSOCK");
}
