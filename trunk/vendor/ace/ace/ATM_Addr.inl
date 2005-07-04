// -*- C++ -*-
//
// ATM_Addr.inl,v 4.1 2004/06/16 07:57:23 jwillemsen Exp


ACE_INLINE
ACE_ATM_Addr::~ACE_ATM_Addr (void)
{
}

// Return the address.

ACE_INLINE void *
ACE_ATM_Addr::get_addr (void) const
{
  ACE_TRACE ("ACE_ATM_Addr::get_addr");
  return (void *) &this->atm_addr_;
}

ACE_INLINE u_char
ACE_ATM_Addr::get_selector (void) const
{
  ACE_TRACE ("ACE_ATM_Addr::get_selector");
#if defined (ACE_HAS_FORE_ATM_XTI)
  return atm_addr_.sap.t_atm_sap_addr.address[ATMNSAP_ADDR_LEN - 1];
#elif defined (ACE_HAS_FORE_ATM_WS2)
  return atm_addr_.satm_number.Addr[ ATM_ADDR_SIZE - 1 ];
#elif defined (ACE_HAS_LINUX_ATM)
  return atm_addr_.sockaddratmsvc.sas_addr.prv[ATM_ESA_LEN - 1];
#else
  return 0;
#endif /* ACE_HAS_FORE_ATM_XTI || ACE_HAS_FORE_ATM_WS2 || ACE_HAS_LINUX_ATM */
}

ACE_INLINE void
ACE_ATM_Addr::set_selector (u_char selector)
{
  ACE_TRACE ("ACE_ATM_Addr::set_selector");
#if defined (ACE_HAS_FORE_ATM_XTI)
  atm_addr_.sap.t_atm_sap_addr.address[ATMNSAP_ADDR_LEN - 1] = selector;
#elif defined (ACE_HAS_FORE_ATM_WS2)
  atm_addr_.satm_number.Addr[ ATM_ADDR_SIZE - 1 ] = selector;
#elif defined (ACE_HAS_LINUX_ATM)
  atm_addr_.sockaddratmsvc.sas_addr.prv[ATM_ESA_LEN - 1] = selector;
#else
  ACE_UNUSED_ARG (selector);
#endif /* ACE_HAS_FORE_ATM_XTI || ACE_HAS_FORE_ATM_WS2 || ACE_HAS_LINUX_ATM */
}