/*
 *  \file ConnectContext.hpp
 *  \brief Signal context for "connect" operation.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_CONNECTCONTEXT_HPP_INCLUDED_
#define PYASYNCHIO_CONNECTCONTEXT_HPP_INCLUDED_

#pragma once

#include <boost/shared_ptr.hpp>
#include <pyasynchio/ConnectSignals.hpp>

namespace pyasynchio {

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


} // namespace pyasynchio

#endif // PYASYNCHIO_CONNECTCONTEXT_HPP_INCLUDED_