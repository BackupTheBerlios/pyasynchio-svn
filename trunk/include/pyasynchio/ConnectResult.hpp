/*!
 *  \file ConnectResult.hpp
 *  \brief Defines interface for "connect result" value (ACE_Asynch_Connect::Result).
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_CONNECTRESULT_HPP_INCLUDED_
#define PYASYNCHIO_CONNECTRESULT_HPP_INCLUDED_

#pragma once
#include <pyasynchio/Result.hpp>

class ACE_Asynch_Connect_Result;

namespace pyasynchio {

struct ConnectResult : Result
{
    void* conn_handle_;

    ConnectResult()
    : Result()
    , conn_handle_()
    {}

    ConnectResult(const ConnectResult &rhs)
    : Result(rhs)
    , conn_handle_(rhs.conn_handle_)
    {}
};


} // namespace pyasynchio

#endif // PYASYNCHIO_CONNECTRESULT_HPP_INCLUDED_