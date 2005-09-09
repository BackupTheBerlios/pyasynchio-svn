#ifndef AOP_IMPL_HPP_INCLUDED_
#define AOP_IMPL_HPP_INCLUDED_

#pragma once

#include <pyasynchio/forwards.hpp>

namespace pyasynchio {

namespace win {

class def_aop_impl 
{
public:
    def_aop_impl() {}
    ~def_aop_impl() {}

    void init() {}
};

class aop_root_impl : public OVERLAPPED, public def_aop_impl
{
public:
    aop_root_impl()
    {
        Internal = InternalHigh = 0;
        Offset = OffsetHigh = 0;
        hEvent = 0;
    }

    ~aop_root_impl() {}
};

template<class aop>
class aop_rw_impl
{
public:
    aop_rw_impl()
    {
    }

    void init() 
    {
        aop * pThis = static_cast<aop*>(this);
        pThis->Offset = static_cast<DWORD>(pThis->off());
        pThis->OffsetHigh = static_cast<DWORD>(pThis->off() >> (sizeof(DWORD) * 8));
    }

    ~aop_rw_impl() {}
};

// Forward
template<class aop> class aop_impl;

// Fallback (no-op)
template<class aop> class aop_impl : public def_aop_impl {};

// Overlapped container.
template<> class aop_impl<aop_root> : public aop_root_impl {};

// Offset handling for files.
template<> class aop_impl<aop_read> : public aop_rw_impl<aop_read> {};
template<> class aop_impl<aop_write> : public aop_rw_impl<aop_write> {};



} // namespace win

} // namespace pyasynchio

#endif // AOP_IMPL_HPP_INCLUDED_