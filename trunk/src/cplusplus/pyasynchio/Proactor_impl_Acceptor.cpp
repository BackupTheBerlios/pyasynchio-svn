/*!
 *  \file Proactor_impl_Acceptor.cpp
 *  \brief Proactor::impl::Acceptor implementation.
 *  \author Vladimir Sukhoy
 */

#include <pyasynchio/Proactor_impl_Acceptor.hpp>
#include <pyasynchio/ResultConversion.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>
#include <pyasynchio/scopeguard.hpp>

namespace pyasynchio {

Proactor::impl::Acceptor::Acceptor(Proactor::impl *pro
    , AbstractAcceptHandlerPtr user_accept_handler
    , ACE_INET_Addr addr)
: pro_(pro)
, user_accept_handler_(user_accept_handler)
{
}

Proactor::impl::AcceptorPtr Proactor::impl::Acceptor::Create(Proactor::impl *pro
    , AbstractAcceptHandlerPtr user_accept_handler
    , ACE_INET_Addr addr)
{
    AcceptorPtr accp(new Acceptor(pro, user_accept_handler, addr));
    accp->thisPtr_ = accp;
    accp->open(addr /* address */
        , 0 /* bytes_to_read */
        , 1 /* pass_addresses */
        , ACE_DEFAULT_BACKLOG /* backlog */
        , 1 /* reuse_addr */
        , &pro->ace_pro_  /* proactor */
        , 1 /* validate_new_connection */
        , 1 /* reissue_accept */);
    return accp;
}


Proactor::impl::Acceptor::~Acceptor()
{
    close();
}

int Proactor::impl::Acceptor::accept(size_t bytes_to_read, const void* act)
{
    AcceptorPtr strong = thisPtr_.lock();
    int rval;
    if((rval = Super::accept(bytes_to_read, act)) == 0) {
        pro_->registerPending(strong);
    }
    return rval;
}

void Proactor::impl::Acceptor::handle_accept (const ACE_Asynch_Accept::Result &result)
{
    ON_BLOCK_EXIT(boost::bind(&Proactor::impl::unregisterPending, pro_, thisPtr_.lock()));
    Super::handle_accept(result);
    AcceptResult ar;
    convert(ar, result);
	user_accept_handler_->conn_completed(ar);
}

int Proactor::impl::Acceptor::validate_connection(
    const ACE_Asynch_Accept::Result &result
    , const ACE_INET_Addr &remote
    , const ACE_INET_Addr &local)
{
    AcceptResult ar;
    convert(ar, result);
	if (user_accept_handler_->conn_validate(ar, remote, local)) {
        return 0;
    }
    return -1;
}

Proactor::impl::StreamHandler* Proactor::impl::Acceptor::make_handler()
{
	AbstractStreamHandlerPtr user_stream_handler = 
		user_accept_handler_->make_stream_handler();
	return pro_->make_impl_stream_handler(user_stream_handler).get();
}

void Proactor::impl::Acceptor::close()
{
    reissue_accept(0);
    cancel();
    if (this->handle() != ACE_INVALID_HANDLE) {
        ACE_HANDLE hval = handle();
        handle(ACE_INVALID_HANDLE);
        ACE_OS::closesocket(hval);
    }
}


} // namespace pyasynchio
