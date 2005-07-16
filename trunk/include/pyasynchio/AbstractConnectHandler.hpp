#ifndef PYASYNCHIO_ABSTRACT_CONNECT_HANDLER_HPP_INCLUDED_
#define PYASYNCHIO_ABSTRACT_CONNECT_HANDLER_HPP_INCLUDED_

#pragma once

#include <pyasynchio/ConnectResult.hpp>
#include <pyasynchio/AbstractStreamHandler.hpp>
#include <boost/shared_ptr.hpp>

namespace pyasynchio {

class AbstractConnectHandler;
typedef boost::shared_ptr<AbstractConnectHandler> AbstractConnectHandlerPtr;

class AbstractConnectHandler
{
protected:
    AbstractConnectHandler() {}
public:
    virtual ~AbstractConnectHandler() {}

    virtual bool conn_validate(const ConnectResult &result
                                , const ACE_INET_Addr &remote
                                , const ACE_INET_Addr &local) = 0;
    virtual void conn_completed(const ConnectResult &result) = 0;
    virtual AbstractStreamHandlerPtr make_stream_handler() = 0;
};


} // namespace pyasynchio

#endif // PYASYNCHIO_ABSTRACT_CONNECT_HANDLER_HPP_INCLUDED_