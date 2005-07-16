/*
 *  \file AbstractStreamHandler.hpp  
 *  \brief Stream handler abstract interface.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_ABSTRACT_STREAM_HANDLER_HPP_INCLUDED_
#define PYASYNCHIO_ABSTRACT_STREAM_HANDLER_HPP_INCLUDED_

#pragma once

#include <pyasynchio/StreamResults.hpp>
#include <boost/shared_ptr.hpp>

class ACE_INET_Addr;

namespace pyasynchio {

class AbstractStreamHandler;
typedef boost::shared_ptr<AbstractStreamHandler> AbstractStreamHandlerPtr;

class AbstractStreamHandler
{
protected:
    AbstractStreamHandler() {}
public:
    virtual ~AbstractStreamHandler() {}

    virtual void notify_endpoints(const ACE_INET_Addr &remote
                                    , const ACE_INET_Addr &local) = 0;
    
    virtual void notify_act(const void *act) = 0;
    virtual void notify_opened(void *handle) = 0;

    virtual void read_completed(const ReadStreamResult &read_result) = 0;
    virtual void write_completed(const WriteStreamResult &write_result) = 0;
};

} // namespace pyasynchio

#endif // PYASYNCHIO_ABSTRACT_STREAM_HANDLER_HPP_INCLUDED_