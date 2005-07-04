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

void Proactor::accept(AcceptContextPtr ctx
                      , const ACE_INET_Addr &addr 
                      , size_t bytesToRead)
{
    pimpl_->accept(ctx, addr, bytesToRead);
}

void Proactor::close(AcceptContextPtr ctx)
{
    pimpl_->close(ctx);
}

void Proactor::connect(ConnectContextPtr ctx
                        , const ACE_INET_Addr &remote
                        , const ACE_INET_Addr &local)
{
    pimpl_->connect(ctx, remote, local);
}

void Proactor::close(ConnectContextPtr ctx)
{
    pimpl_->close(ctx);
}

void Proactor::write(StreamContextPtr ctx
                    , const buf &data
                    , const void *act
                    , int priority
                    , int signal)
{
    pimpl_->write(ctx, data, act, priority, signal);
}

void Proactor::cancelWrite(StreamContextPtr ctx)
{
    pimpl_->cancelWrite(ctx);
}

void Proactor::read(StreamContextPtr ctx
                    , size_t bytes
                    , const void *act
                    , int priority
                    , int signal)
{
    pimpl_->read(ctx, bytes, act, priority, signal);
}

void Proactor::cancelRead(StreamContextPtr ctx)
{
    pimpl_->cancelRead(ctx);
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

void Proactor::close(StreamContextPtr ctx)
{
    pimpl_->close(ctx);
}

} // namespace pyasynchio
