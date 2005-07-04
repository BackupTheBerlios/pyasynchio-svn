/*!
 *  \file AcceptResult.hpp
 *  \brief Defines interface for "accept result" value (ACE_Asynch_Accept::Result).
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_ACCEPTRESULT_HPP_INCLUDED_
#define PYASYNCHIO_ACCEPTRESULT_HPP_INCLUDED_

#pragma once
#include <pyasynchio/Result.hpp>
#include <pyasynchio/primitive.hpp>
#include <boost/bind.hpp>

class ACE_Asynch_Accept_Result;

namespace pyasynchio {

struct AcceptResult : Result
{
    buf message_block_;
    void* listen_handle_;
    void* accept_handle_;

    AcceptResult()
    : Result()
    , listen_handle_()
    , accept_handle_()
    {}

    AcceptResult(const AcceptResult &rhs)
    : Result(rhs)
    , message_block_(rhs.message_block_)
    , listen_handle_(rhs.listen_handle_)
    , accept_handle_(rhs.accept_handle_)
    {}
};

} // namespace pyasynchio

#endif // PYASYNCHIO_ACCEPTRESULT_HPP_INCLUDED_