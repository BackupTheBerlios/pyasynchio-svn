/*
 *  \file AcceptContext.hpp
 *  \brief Signal context for "accept" operation.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_ACCEPTCONTEXT_HPP_INCLUDED_
#define PYASYNCHIO_ACCEPTCONTEXT_HPP_INCLUDED_

#pragma once

#include <boost/shared_ptr.hpp>
#include <pyasynchio/AcceptSignals.hpp>

namespace pyasynchio {

class AcceptContext;
typedef boost::shared_ptr<AcceptContext> AcceptContextPtr;

class AcceptContext
{
public:
    ValidateAcceptSignal sigValidate_;
    AcceptStreamSignal sigStream_;
    AcceptCompleteSignal sigComplete_;

    static AcceptContextPtr Create() 
    {
        return AcceptContextPtr(new AcceptContext());
    }
protected:
    AcceptContext() {}
};

} // namespace pyasynchio

#endif // PYASYNCHIO_ACCEPTCONTEXT_HPP_INCLUDED_