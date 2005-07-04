/*
 *  \file ConnectContext.hpp
 *  \brief Signal context for "connect" operation.
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_CONNECTCONTEXT_HPP_INCLUDED_
#define SPACE_TRANSPORT_CONNECTCONTEXT_HPP_INCLUDED_

#pragma once

#include <boost/shared_ptr.hpp>
#include <space/transport/ConnectSignals.hpp>

namespace space {

namespace transport {

class ConnectContext;
typedef boost::shared_ptr<ConnectContext> ConnectContextPtr;
typedef boost::weak_ptr<ConnectContext> ConnectContextWeakPtr;

class ConnectContext
{
public:
    ValidateConnectSignal sigValidate_;
	ConnectStreamSignal sigStream_;
	ConnectCompleteSignal sigComplete_;

    static ConnectContextPtr Create() 
    {
        return ConnectContextPtr(new ConnectContext());
    }
protected:
    ConnectContext() {}
};

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_CONNECTCONTEXT_HPP_INCLUDED_