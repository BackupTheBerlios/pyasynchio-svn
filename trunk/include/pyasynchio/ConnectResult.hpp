/*!
 *  \file ConnectResult.hpp
 *  \brief Defines interface for "connect result" value (ACE_Asynch_Connect::Result).
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_CONNECTRESULT_HPP_INCLUDED_
#define SPACE_TRANSPORT_CONNECTRESULT_HPP_INCLUDED_

#pragma once
#include <space/transport/Result.hpp>

class ACE_Asynch_Connect_Result;

namespace space {

namespace transport {

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

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_CONNECTRESULT_HPP_INCLUDED_