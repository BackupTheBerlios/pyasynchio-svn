/*!
 *  \file AcceptResult.hpp
 *  \brief Defines interface for "accept result" value (ACE_Asynch_Accept::Result).
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_ACCEPTRESULT_HPP_INCLUDED_
#define SPACE_TRANSPORT_ACCEPTRESULT_HPP_INCLUDED_

#pragma once
#include <space/transport/Result.hpp>
#include <space/primitive.hpp>
#include <boost/bind.hpp>

class ACE_Asynch_Accept_Result;

namespace space {

namespace transport {

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

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_ACCEPTRESULT_HPP_INCLUDED_