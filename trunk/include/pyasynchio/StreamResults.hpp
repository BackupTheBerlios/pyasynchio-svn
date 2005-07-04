/*!
 *  \file StreamResults.hpp
 *  \brief Defines interface for "stream results" 
 *  values (ACE_Asynch_Read_Stream::Result and ACE_Asynch_Write_Stream::Result).
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_STREAMRESULTS_HPP_INCLUDED_
#define SPACE_TRANSPORT_STREAMRESULTS_HPP_INCLUDED_

#pragma once

#include <space/transport/Result.hpp>
#include <boost/bind.hpp>

class ACE_Asynch_Read_Stream_Result;
class ACE_Asynch_Write_Stream_Result;

namespace space {

namespace transport {

struct ReadStreamResult : Result
{
    buf data_;
    void* handle_;
    
    ReadStreamResult()
	: Result()
	, handle_()
	{}

	ReadStreamResult(const ReadStreamResult &rhs)
	: Result(rhs)
	, handle_(rhs.handle_)
	, data_(rhs.data_)
	{}
};

struct WriteStreamResult : Result
{
    buf data_;
    void* handle_;
    
    WriteStreamResult()
	: handle_()
	{}

	WriteStreamResult(const WriteStreamResult &rhs)
	: Result(rhs)
	, data_(rhs.data_)
	, handle_(rhs.handle_)
	{}
};

} // namespace transport

} // namespace space


#endif // SPACE_TRANSPORT_STREAMRESULTS_HPP_INCLUDED_