/*!
 *  \file Proactor_impl_Acceptor.hpp
 *  \brief Proactor::impl::Acceptor interface.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_PROACTOR_IMPL_ACCEPTOR_HPP_INCLUDED_
#define PYASYNCHIO_PROACTOR_IMPL_ACCEPTOR_HPP_INCLUDED_

#pragma once
 
#include <ace/Asynch_Acceptor.h>
#include <pyasynchio/Proactor_impl.hpp>
#include <pyasynchio/Proactor_impl_StreamHandler.hpp>

namespace pyasynchio {

class Proactor::impl::Acceptor 
: public ACE_Asynch_Acceptor<StreamHandler>
{
public:
    typedef ACE_Asynch_Acceptor<StreamHandler> Super;
    virtual ~Acceptor();
    
    int validate_connection (const ACE_Asynch_Accept::Result &result
        , const ACE_INET_Addr &remote
        , const ACE_INET_Addr &local);
    int accept(size_t bytes_to_read, const void* act);

    void handle_accept (const ACE_Asynch_Accept::Result &result);

    void close();
    
    StreamHandler* make_handler();

    static AcceptorPtr Create(Proactor::impl *pro
        , AbstractAcceptHandlerPtr user_accept_handler
        , ACE_INET_Addr addr);

protected:
    Acceptor(Proactor::impl *pro
        , AbstractAcceptHandlerPtr user_accept_handler
        , ACE_INET_Addr addr);

private:
    Proactor::impl *pro_;
    AbstractAcceptHandlerPtr user_accept_handler_;
    AcceptorWeakPtr thisPtr_;
};




} // namespace pyasynchio

#endif // PYASYNCHIO_PROACTOR_IMPL_ACCEPTOR_HPP_INCLUDED_