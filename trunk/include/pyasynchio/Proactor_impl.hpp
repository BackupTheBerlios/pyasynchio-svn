/*!
 *  \file Proactor_impl.hpp
 *  \brief PIMPL idiom implementation header for proactor wrapper.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_PROACTOR_IMPL_HPP_INCLUDED_
#define PYASYNCHIO_PROACTOR_IMPL_HPP_INCLUDED_

#pragma once

#include <pyasynchio/Proactor.hpp>
#include <pyasynchio/buf.hpp>
#include <pyasynchio/safe_map.hpp>
#include <ace/Task_T.h>
#include <ace/Proactor.h>
#include <ace/Manual_Event.h>
#include <boost/function.hpp>
#include <set>

namespace pyasynchio {

/************************************************************************/
/* Proactor implementation (PIMPL)                                      */
/************************************************************************/
struct less_without_shared_ownership_check
{
    bool operator()(AbstractStreamHandlerPtr const & lhs
        , AbstractStreamHandlerPtr const & rhs) const
    {
        return lhs.get() < rhs.get();
    }
};
    

class Proactor::impl : public ACE_Task<ACE_MT_SYNCH>
{
public:
    impl();
    ~impl();

    void schedule_timer(ACE_Time_Value time, AbstractTimerHandlerPtr elapsed);
    void cancel_timer(AbstractTimerHandlerPtr elapsed);

    void open_stream_accept(AbstractAcceptHandlerPtr user_accept_handler 
        , const ACE_INET_Addr &addr 
        , size_t bytesToRead /* = 0 */);
    void open_stream_connect(AbstractConnectHandlerPtr user_connect_handler 
        , const ACE_INET_Addr &remote 
        , const ACE_INET_Addr &local /* = ANY_INTERFACE */);
    void close_stream_accept(AbstractAcceptHandlerPtr user_accept_handler);
    void close_stream_connect(AbstractConnectHandlerPtr user_connect_handler);
    void close_active_stream(AbstractStreamHandlerPtr user_stream_handler);

    void handle_events(ACE_Time_Value delay);
    void handle_events();
    
    void open_stream_write(AbstractStreamHandlerPtr user_stream_handler
        , const buf &data
        , const void *act
        , int priority
        , int signal);
    void cancel_stream_write(AbstractStreamHandlerPtr user_stream_handler);
    void open_stream_read(AbstractStreamHandlerPtr user_stream_handler
        , size_t count
        , const void *act
        , int priority
        , int signal);
    void cancel_stream_read(AbstractStreamHandlerPtr user_stream_handler);
    
    class StreamHandler;
    typedef boost::shared_ptr<StreamHandler> StreamHandlerPtr;
    typedef boost::weak_ptr<StreamHandler> StreamHandlerWeakPtr;

    class Acceptor;
    typedef boost::shared_ptr<Acceptor> AcceptorPtr;
    typedef boost::weak_ptr<Acceptor> AcceptorWeakPtr;
    
    class Connector;
    typedef boost::shared_ptr<Connector> ConnectorPtr;
    typedef boost::weak_ptr<Connector> ConnectorWeakPtr;

    class TimerHandler;
    typedef boost::shared_ptr<TimerHandler> TimerHandlerPtr;

    typedef boost::shared_ptr<ACE_Handler> ACE_HandlerPtr;

private:
    void scheduleBlocking(boost::function<void ()>);
    Proactor::impl::StreamHandlerPtr make_impl_stream_handler(
        AbstractStreamHandlerPtr user_stream_handler);

    void registerPending(ACE_HandlerPtr pending);
    void unregisterPending(ACE_HandlerPtr pending);

    ACE_Proactor ace_pro_;

    typedef std::multimap<AbstractAcceptHandlerPtr, AcceptorPtr> Acceptors;
    Acceptors acceptors_;
    ACE_RW_Thread_Mutex acceptors_rwmutex_;
    
    typedef std::multimap<AbstractConnectHandlerPtr, ConnectorPtr> Connectors;
    Connectors connectors_;
    ACE_RW_Thread_Mutex connectors_rwmutex_;

    typedef safe_map<AbstractStreamHandlerPtr
        , StreamHandlerPtr
        , less_without_shared_ownership_check> Streams;
    Streams streams_;
    ACE_RW_Thread_Mutex streams_rwmutex_;

    typedef std::multimap<AbstractTimerHandlerPtr, TimerHandlerPtr> TimerHandlers;
    TimerHandlers ths_; 
    ACE_RW_Thread_Mutex ths_rwmutex_;

    typedef std::multiset<ACE_HandlerPtr> PendingHandlers;
    PendingHandlers pending_;
    ACE_RW_Thread_Mutex pending_rwmutex_;    
};



} // namespace pyasynchio


#endif // PYASYNCHIO_PROACTOR_IMPL_HPP_INCLUDED_