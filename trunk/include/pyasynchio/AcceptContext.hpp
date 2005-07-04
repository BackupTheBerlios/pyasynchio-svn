/*
 *  \file AcceptContext.hpp
 *  \brief Signal context for "accept" operation.
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_ACCEPTCONTEXT_HPP_INCLUDED_
#define SPACE_TRANSPORT_ACCEPTCONTEXT_HPP_INCLUDED_

#pragma once

#include <boost/shared_ptr.hpp>
#include <space/transport/AcceptSignals.hpp>

namespace space {

namespace transport {

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

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_ACCEPTCONTEXT_HPP_INCLUDED_