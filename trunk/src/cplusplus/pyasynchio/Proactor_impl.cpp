/*
 *  \file Proactor_impl.cpp
 *  \brief PIMPL idiom implementation for pyasynchio::Proactor.
 *  \author Vladimir Sukhoy
 */

#include <pyasynchio/Proactor_impl.hpp>
#include <pyasynchio/Proactor_impl_TimerHandler.hpp>
#include <pyasynchio/Proactor_impl_Acceptor.hpp>
#include <pyasynchio/Proactor_impl_Connector.hpp>
#include <ace/Synch.h>
#include <ace/Thread.h>

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
    ACE_GUARD(ACE_Thread_Mutex, g, thsMutex_);
    ths_.insert(std::make_pair(elapsed, th));
    ace_pro_.schedule_timer(*th, NULL, delay);
}

void Proactor::impl::cancelTimer(TimerSignalPtr elapsed)
{
    std::pair<TimerHandlers::iterator, TimerHandlers::iterator> eqr;
    ACE_GUARD(ACE_Thread_Mutex, g, thsMutex_);
    eqr = ths_.equal_range(elapsed);
    TimerHandlers::iterator thi;
    for (thi = eqr.first; thi != eqr.second; ++thi) {
        thi->second->cancel();
    }
}

void Proactor::impl::accept(AcceptContextPtr ctx
                            , ACE_INET_Addr addr
                            , size_t bytesToRead)
{
    AcceptorPtr acc = Acceptor::Create(this, ctx, addr);
    {
        ACE_GUARD(ACE_Thread_Mutex, g, acceptorsMutex_);
        acceptors_.insert(std::make_pair(ctx, acc));
    }
}

void Proactor::impl::connect(ConnectContextPtr ctx
                            , ACE_INET_Addr remote
                            , ACE_INET_Addr local)
{
    ConnectorPtr conn = Connector::Create(this, ctx, remote, local);
    {
        ACE_GUARD(ACE_Thread_Mutex, g, connectorsMutex_);
        connectors_.insert(std::make_pair(ctx, conn));
    }
}

void Proactor::impl::close(ConnectContextPtr ctx)
{
    std::pair<Connectors::iterator, Connectors::iterator> eqr;
    Connectors::iterator ci;
    
    {
        ACE_GUARD(ACE_Thread_Mutex, g, connectorsMutex_);
        eqr = connectors_.equal_range(ctx);
        for (ci = eqr.first; ci != eqr.second; ++ci) {
            ci->second->close();
        }
        connectors_.erase(ctx);
    }
}

void Proactor::impl::close(AcceptContextPtr ctx)
{
    std::pair<Acceptors::iterator, Acceptors::iterator> eqr;
    Acceptors::iterator ai;
    
    {
        ACE_GUARD(ACE_Thread_Mutex, g, acceptorsMutex_);
        eqr = acceptors_.equal_range(ctx);
        for (ai = eqr.first; ai != eqr.second; ++ai) {
            ai->second->close();
        }
        acceptors_.erase(ctx);
    }
}

void Proactor::impl::close(StreamContextPtr ctx)
{
    ACE_GUARD(ACE_Thread_Mutex, g, streamsMutex_);
    streams_[ctx]->close();
    streams_.erase(ctx);
}

void Proactor::impl::write(StreamContextPtr ctx
                            , const buf &data
                            , const void *act
                            , int priority
                            , int signal)
{
    ACE_GUARD(ACE_Thread_Mutex, g, streamsMutex_);
    streams_[ctx]->write(data, act, priority, signal);
}

void Proactor::impl::cancelWrite(StreamContextPtr ctx)
{
    ACE_GUARD(ACE_Thread_Mutex, g, streamsMutex_);
    scheduleBlocking(boost::bind(&StreamHandler::cancelWrite, streams_[ctx].get()));
}

void Proactor::impl::read(StreamContextPtr ctx
                            , size_t count
                            , const void *act
                            , int priority
                            , int signal)
{
    ACE_GUARD(ACE_Thread_Mutex, g, streamsMutex_);
    streams_[ctx]->read(count, act, priority, signal);
}

void Proactor::impl::cancelRead(StreamContextPtr ctx)
{
    ACE_GUARD(ACE_Thread_Mutex, g, streamsMutex_);
    scheduleBlocking(boost::bind(&StreamHandler::cancelRead, streams_[ctx].get()));
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

std::pair<StreamContextPtr, Proactor::impl::StreamHandlerPtr> Proactor::impl::makeStream()
{
    StreamContextPtr cp = StreamContext::Create();
    StreamHandlerPtr sh = StreamHandler::Create(this, cp);
    {
        ACE_GUARD_RETURN(ACE_Thread_Mutex, g, streamsMutex_, std::make_pair(cp, sh));
        streams_.insert(std::make_pair(cp, sh));
    }
    return std::make_pair(cp, sh);
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
