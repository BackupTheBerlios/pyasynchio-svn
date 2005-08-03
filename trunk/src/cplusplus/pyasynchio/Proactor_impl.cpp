/*
 *  \file Proactor_impl.cpp
 *  \brief PIMPL idiom implementation for pyasynchio::Proactor.
 *  \author Vladimir Sukhoy
 */

#include <pyasynchio/detail/fix_ace_warnings.hpp>
#include <ace/Synch.h>
#include <ace/Thread.h>
#include <pyasynchio/Proactor_impl.hpp>
#include <pyasynchio/Proactor_impl_TimerHandler.hpp>
#include <pyasynchio/Proactor_impl_Acceptor.hpp>
#include <pyasynchio/Proactor_impl_Connector.hpp>

namespace pyasynchio {

Proactor::impl::impl()
{}

Proactor::impl::~impl()
{}

void Proactor::impl::schedule_timer(ACE_Time_Value delay
                                    , AbstractTimerHandlerPtr elapsed_handler)
{
    TimerHandlerPtr th(new TimerHandler(this, elapsed_handler));
    ths_.insert(std::make_pair(elapsed_handler, th));
    ace_pro_.schedule_timer(*th, NULL, delay);
}

void Proactor::impl::cancel_timer(AbstractTimerHandlerPtr elapsed_handler)
{
    std::pair<TimerHandlers::iterator, TimerHandlers::iterator> eqr;
    eqr = ths_.equal_range(elapsed_handler);
    TimerHandlers::iterator thi;
    for (thi = eqr.first; thi != eqr.second; ++thi) {
        thi->second->cancel();
    }
}

void Proactor::impl::open_stream_accept(AbstractAcceptHandlerPtr user_accept_handler
                            , const ACE_INET_Addr &addr
                            , size_t bytesToRead)
{
    AcceptorPtr acc = Acceptor::Create(this, user_accept_handler, addr);
    ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, write_guard, acceptors_rwmutex_);
    acceptors_.insert(std::make_pair(user_accept_handler, acc));
}

void Proactor::impl::open_stream_connect(AbstractConnectHandlerPtr user_connect_handler
                            , const ACE_INET_Addr &remote
                            , const ACE_INET_Addr &local)
{
    ConnectorPtr conn = Connector::Create(this, user_connect_handler, remote, local);
	ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, write_guard, connectors_rwmutex_);
    connectors_.insert(std::make_pair(user_connect_handler, conn));
}

void Proactor::impl::close_stream_connect(AbstractConnectHandlerPtr user_connect_handler)
{
    std::pair<Connectors::iterator, Connectors::iterator> eqr;
    Connectors::iterator ci;
    
	ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, write_guard, connectors_rwmutex_);
    eqr = connectors_.equal_range(user_connect_handler);
    for (ci = eqr.first; ci != eqr.second; ++ci) {
        ci->second->close();
    }
    connectors_.erase(user_connect_handler);
}

void Proactor::impl::close_stream_accept(AbstractAcceptHandlerPtr user_accept_handler)
{
    std::pair<Acceptors::iterator, Acceptors::iterator> eqr;
    Acceptors::iterator ai;
    
	ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, write_guard, acceptors_rwmutex_);
    eqr = acceptors_.equal_range(user_accept_handler);
    for (ai = eqr.first; ai != eqr.second; ++ai) {
        ai->second->close();
    }
    acceptors_.erase(user_accept_handler);
}

void Proactor::impl::close_active_stream(AbstractStreamHandlerPtr user_stream_handler)
{
	{
		ACE_READ_GUARD(ACE_RW_Thread_Mutex, read_guard, streams_rwmutex_);
	    streams_[user_stream_handler]->close();
	}
	ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, write_guard, streams_rwmutex_);
    streams_.erase(user_stream_handler);
}

void Proactor::impl::open_stream_write(AbstractStreamHandlerPtr user_stream_handler
                            , const buf &data
                            , const void *act
                            , int priority
                            , int signal)
{
	ACE_READ_GUARD(ACE_RW_Thread_Mutex, read_guard, streams_rwmutex_);
    streams_[user_stream_handler]->write(data, act, priority, signal);
}

void Proactor::impl::cancel_stream_write(AbstractStreamHandlerPtr user_stream_handler)
{
	ACE_READ_GUARD(ACE_RW_Thread_Mutex, read_guard, streams_rwmutex_);
	streams_[user_stream_handler]->cancelWrite();
    //scheduleBlocking(boost::bind(&StreamHandler::cancelWrite
    //    , streams_[user_stream_handler].get()));
}

void Proactor::impl::open_stream_read(AbstractStreamHandlerPtr user_stream_handler
                            , size_t count
                            , const void *act
                            , int priority
                            , int signal)
{
	ACE_READ_GUARD(ACE_RW_Thread_Mutex, read_guard, streams_rwmutex_);
    streams_[user_stream_handler]->read(count, act, priority, signal);
}

void Proactor::impl::cancel_stream_read(AbstractStreamHandlerPtr user_stream_handler)
{
	ACE_READ_GUARD(ACE_RW_Thread_Mutex, read_guard, streams_rwmutex_);
	streams_[user_stream_handler]->cancelRead();
    //scheduleBlocking(boost::bind(&StreamHandler::cancelRead
    //    , streams_[user_stream_handler].get()));
}

void Proactor::impl::scheduleBlocking(boost::function<void()> fn)
{
    fn();
}


Proactor::impl::StreamHandlerPtr 
Proactor::impl::make_impl_stream_handler(AbstractStreamHandlerPtr user_stream_handler)
{
    Proactor::impl::StreamHandlerPtr impl_stream_handler = 
        Proactor::impl::StreamHandler::Create(this, user_stream_handler);
	ACE_WRITE_GUARD_RETURN(ACE_RW_Thread_Mutex, write_guard, streams_rwmutex_, impl_stream_handler);
    streams_.insert(std::make_pair(user_stream_handler, impl_stream_handler));
    return impl_stream_handler;
}

void Proactor::impl::handle_events(ACE_Time_Value delay)
{
    ace_pro_.handle_events(delay);
}

void Proactor::impl::handle_events()
{
    ace_pro_.handle_events();
}

void Proactor::impl::registerPending(ACE_HandlerPtr pending)
{
	ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, write_guard, pending_rwmutex_);
    pending_.insert(pending);
}

void Proactor::impl::unregisterPending(ACE_HandlerPtr pending)
{
	ACE_WRITE_GUARD(ACE_RW_Thread_Mutex, write_guard, pending_rwmutex_);
    pending_.erase(pending_.find(pending));
}

} // namespace pyasynchio
