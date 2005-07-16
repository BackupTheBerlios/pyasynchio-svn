/*
 *  \file Proactor_impl.cpp
 *  \brief PIMPL idiom implementation for pyasynchio::Proactor.
 *  \author Vladimir Sukhoy
 */

#include <ace/Synch.h>
#include <ace/Thread.h>
#include <pyasynchio/Proactor_impl.hpp>
#include <pyasynchio/Proactor_impl_TimerHandler.hpp>
#include <pyasynchio/Proactor_impl_Acceptor.hpp>
#include <pyasynchio/Proactor_impl_Connector.hpp>

namespace pyasynchio {

Proactor::impl::impl(bool active)
: done_(false)
, active_(active)
{
    if(active) {
        this->activate();
        got_ace_pro_thr_.wait();
    }
    else {
        ace_pro_thr_ = ACE_Thread::self();
    }
}


Proactor::impl::~impl()
{
    shutdown();
}

void Proactor::impl::shutdown()
{
    if(active_) {
        if(!done_) {
            done_ = true;
            this->wait();
        }
        active_ = false;
    }
}


int Proactor::impl::svc()
{
    ace_pro_thr_ = ACE_Thread::self();
    got_ace_pro_thr_.signal();
    static ACE_Time_Value interval(0, 10000);
    while(!done_) {
        ace_pro_.handle_events(interval);
    }
    return 0;
}

void Proactor::impl::scheduleTimer(ACE_Time_Value delay, TimerSignalPtr elapsed)
{
    TimerHandlerPtr th(new TimerHandler(this, elapsed));
    ths_.insert(std::make_pair(elapsed, th));
    ace_pro_.schedule_timer(*th, NULL, delay);
}

void Proactor::impl::cancelTimer(TimerSignalPtr elapsed)
{
    std::pair<TimerHandlers::iterator, TimerHandlers::iterator> eqr;
    eqr = ths_.equal_range(elapsed);
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
	acceptors_.insert(std::make_pair(user_accept_handler, acc));
}

void Proactor::impl::open_stream_connect(AbstractConnectHandlerPtr user_connect_handler
                            , const ACE_INET_Addr &remote
                            , const ACE_INET_Addr &local)
{
    ConnectorPtr conn = Connector::Create(this, user_connect_handler, remote, local);
    connectors_.insert(std::make_pair(user_connect_handler, conn));
}

void Proactor::impl::close_stream_connect(AbstractConnectHandlerPtr user_connect_handler)
{
    std::pair<Connectors::iterator, Connectors::iterator> eqr;
    Connectors::iterator ci;
    
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
    
    eqr = acceptors_.equal_range(user_accept_handler);
    for (ai = eqr.first; ai != eqr.second; ++ai) {
        ai->second->close();
    }
    acceptors_.erase(user_accept_handler);
}

void Proactor::impl::close_active_stream(AbstractStreamHandlerPtr user_stream_handler)
{
    streams_[user_stream_handler]->close();
    streams_.erase(user_stream_handler);
}

void Proactor::impl::open_stream_write(AbstractStreamHandlerPtr user_stream_handler
                            , const buf &data
                            , const void *act
                            , int priority
                            , int signal)
{
    streams_[user_stream_handler]->write(data, act, priority, signal);
}

void Proactor::impl::cancel_stream_write(AbstractStreamHandlerPtr user_stream_handler)
{
    scheduleBlocking(boost::bind(&StreamHandler::cancelWrite
		, streams_[user_stream_handler].get()));
}

void Proactor::impl::open_stream_read(AbstractStreamHandlerPtr user_stream_handler
                            , size_t count
                            , const void *act
                            , int priority
                            , int signal)
{
    streams_[user_stream_handler]->read(count, act, priority, signal);
}

void Proactor::impl::cancel_stream_read(AbstractStreamHandlerPtr user_stream_handler)
{
    scheduleBlocking(boost::bind(&StreamHandler::cancelRead
		, streams_[user_stream_handler].get()));
}

void Proactor::impl::scheduleBlocking(boost::function<void()> fn)
{
    if (!active_ || ACE_Thread::self() == ace_pro_thr_) {
        // If we're actually in ACE Proactor thread.
        fn();
        return;
    }

    // We are in other thread, so we assemble some stuff :).
    ACE_Manual_Event evt;
    TimerSignalPtr elapsed = TimerSignal::Create();
    elapsed->connect(0, boost::bind(fn) );
    elapsed->connect(1, boost::bind(&ACE_Manual_Event::signal, &evt));
    scheduleTimer(ACE_Time_Value(0,0), elapsed);
    evt.wait();
}


Proactor::impl::StreamHandlerPtr 
Proactor::impl::make_impl_stream_handler(AbstractStreamHandlerPtr user_stream_handler)
{
	Proactor::impl::StreamHandlerPtr impl_stream_handler = 
		Proactor::impl::StreamHandler::Create(this, user_stream_handler);
	{
		ACE_GUARD_RETURN(ACE_Thread_Mutex, g, streamsMutex_, impl_stream_handler);
		streams_.insert(std::make_pair(user_stream_handler, impl_stream_handler));
	}
	return impl_stream_handler;
}

void Proactor::impl::handleEvents(ACE_Time_Value delay)
{
    ace_pro_.handle_events(delay);
}

void Proactor::impl::handleEvents()
{
    ace_pro_.handle_events();
}

void Proactor::impl::registerPending(ACE_HandlerPtr pending)
{
    ACE_GUARD(ACE_Thread_Mutex, g, pendingMutex_);
    pending_.insert(pending);
}

void Proactor::impl::unregisterPending(ACE_HandlerPtr pending)
{
    ACE_GUARD(ACE_Thread_Mutex, g, pendingMutex_);
    pending_.erase(pending_.find(pending));
}

} // namespace pyasynchio
