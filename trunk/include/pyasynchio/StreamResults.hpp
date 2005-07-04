/*!
 *  \file StreamResults.hpp
 *  \brief Defines interface for "stream results" 
 *  values (ACE_Asynch_Read_Stream::Result and ACE_Asynch_Write_Stream::Result).
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_STREAMRESULTS_HPP_INCLUDED_
#define PYASYNCHIO_STREAMRESULTS_HPP_INCLUDED_

#pragma once

#include <pyasynchio/Result.hpp>
#include <boost/bind.hpp>

class ACE_Asynch_Read_Stream_Result;
class ACE_Asynch_Write_Stream_Result;

namespace pyasynchio {

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

} // namespace pyasynchio


#endif // PYASYNCHIO_STREAMRESULTS_HPP_INCLUDED_