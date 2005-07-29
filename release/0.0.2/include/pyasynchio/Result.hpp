/*!
 *  \file Result.hpp
 *
 *  \brief Defines interface for "result" value.
 *
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_RESULT_HPP_INCLUDED_
#define PYASYNCHIO_RESULT_HPP_INCLUDED_

#pragma once

#include <pyasynchio/primitive.hpp>
#include <pyasynchio/buf.hpp>
#include <boost/function.hpp>

class ACE_Asynch_Result;

namespace pyasynchio {

struct Result
{
    const void *act_;
    int success_;
    const void * ckey_;
    unsigned long error_;
    void* event_;
    integer offset_;
    int priority_;
    int signo_;

    Result()
    : act_()
    , success_()
    , ckey_()
    , error_()
    , event_()
    , offset_()
    , priority_()
    , signo_()
    {}

    Result(const Result &rhs)
    : act_(rhs.act_)
    , success_(rhs.success_)
    , ckey_(rhs.ckey_)
    , error_(rhs.error_)
    , event_(rhs.event_)
    , offset_(rhs.offset_)
    , priority_(rhs.priority_)
    , signo_(rhs.signo_)
    {}

    static buf FromFunctor(boost::function<char*()> fn, size_t count)
    {
        buf rval(count);
        if (count == 0) return rval;
        char *pdata = fn();

        size_t idx;
        for (idx = 0; idx < count; ++idx) {
            rval[idx] = pdata[idx];
        }
        return rval;
    }
};


} // namespace pyasynchio


#endif // PYASYNCHIO_RESULT_HPP_INCLUDED_