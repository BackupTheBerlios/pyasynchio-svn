/*!
 *  \file Proactor.cpp
 *  \brief Exploratory wrappers over ACE_Proactor. 
 *  Implementation of pyasynchio::Proactor module.
 *  \author Vladimir Sukhoy
 */

#include <ace/Proactor.h>
#include <ace/Asynch_Acceptor.h>
#include <ace/INET_Addr.h>
#include <pyasynchio/Proactor.hpp>
#include <pyasynchio/Proactor_impl.hpp>

namespace pyasynchio {

const ACE_INET_Addr Proactor::ANY_INTERFACE(static_cast<u_short>(0));

/************************************************************************/
/* Proactor facade (PIMPL)                                              */
/************************************************************************/

Proactor::Proactor(bool active)
: pimpl_(new impl(active))
{
}

Proactor::~Proactor() 
{
    pimpl_->shutdown();
}

void Proactor::scheduleTimer(const ACE_Time_Value &delay, TimerSignalPtr elapsed)
{
    pimpl_->scheduleTimer(delay, elapsed);
}

void Proactor::cancelTimer(TimerSignalPtr elapsed)
{
    pimpl_->cancelTimer(elapsed);
}

void Proactor::open_stream_accept(AbstractAcceptHandlerPtr user_accept_handler
                      , const ACE_INET_Addr &addr 
                      , size_t bytesToRead)
{
    pimpl_->open_stream_accept(user_accept_handler, addr, bytesToRead);
}

void Proactor::close_stream_accept(AbstractAcceptHandlerPtr user_accept_handler)
{
    pimpl_->close_stream_accept(user_accept_handler);
}

void Proactor::open_stream_connect(AbstractConnectHandlerPtr user_connect_handler
                        , const ACE_INET_Addr &remote
                        , const ACE_INET_Addr &local)
{
    pimpl_->open_stream_connect(user_connect_handler, remote, local);
}

void Proactor::close_stream_connect(AbstractConnectHandlerPtr user_connect_handler)
{
    pimpl_->close_stream_connect(user_connect_handler);
}

void Proactor::open_stream_write(AbstractStreamHandlerPtr user_stream_handler
                    , const buf &data
                    , const void *act
                    , int priority
                    , int signal)
{
    pimpl_->open_stream_write(user_stream_handler, data, act, priority, signal);
}

void Proactor::cancel_stream_write(AbstractStreamHandlerPtr user_stream_handler)
{
    pimpl_->cancel_stream_write(user_stream_handler);
}

void Proactor::open_stream_read(AbstractStreamHandlerPtr user_stream_handler
                    , size_t bytes
                    , const void *act
                    , int priority
                    , int signal)
{
    pimpl_->open_stream_read(user_stream_handler, bytes, act, priority, signal);
}

void Proactor::cancel_stream_read(AbstractStreamHandlerPtr user_stream_handler)
{
    pimpl_->cancel_stream_read(user_stream_handler);
}

void Proactor::handleEvents(const ACE_Time_Value &delay)
{
    pimpl_->handleEvents(delay);
}

void Proactor::handleEvents()
{
    pimpl_->handleEvents();
}

void Proactor::shutdown()
{
    pimpl_->shutdown();
}

void Proactor::close_active_stream(AbstractStreamHandlerPtr user_stream_handler)
{
    pimpl_->close_active_stream(user_stream_handler);
}

} // namespace pyasynchio
