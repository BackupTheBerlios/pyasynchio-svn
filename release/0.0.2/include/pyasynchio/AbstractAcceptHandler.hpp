/*  
 *  \file AbstractAcceptHandler.hpp
 *  \brief Signal handler abstract interface.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_ABSTRACT_ACCEPT_HANDLER_HPP_INCLUDED_
#define PYASYNCHIO_ABSTRACT_ACCEPT_HANDLER_HPP_INCLUDED_

#pragma once

#include <pyasynchio/AcceptResult.hpp>
#include <pyasynchio/AbstractStreamHandler.hpp>
#include <boost/shared_ptr.hpp>

namespace pyasynchio {

class AbstractAcceptHandler;
typedef boost::shared_ptr<AbstractAcceptHandler> AbstractAcceptHandlerPtr;

class AbstractAcceptHandler
{
public:
    AbstractAcceptHandler() {}
    virtual ~AbstractAcceptHandler() {}

    virtual bool conn_validate(const AcceptResult &result
                        , const ACE_INET_Addr &remote
                        , const ACE_INET_Addr &local) = 0;
    virtual void conn_completed(const AcceptResult &result) = 0;
    virtual AbstractStreamHandlerPtr make_stream_handler() = 0;
};

} // namespace pyasynchio

#endif // PYASYNCHIO_ABSTRACT_ACCEPT_HANDLER_HPP_INCLUDED_