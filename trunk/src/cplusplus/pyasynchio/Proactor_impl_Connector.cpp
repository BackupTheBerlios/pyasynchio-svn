/*!
 *` \file Proactor_impl_Connector.cpp
 *  \brief Proactor::impl::Connector implementation module
 *  \author Vladimir Sukhoy
 */

#include <pyasynchio/Proactor_impl_Connector.hpp>
#include <pyasynchio/ResultConversion.hpp>
#include <pyasynchio/scopeguard.hpp>

namespace pyasynchio {

Proactor::impl::Connector::Connector(Proactor::impl *pro
            , ConnectContextPtr ctx)
: pro_(pro)
, ctx_(ctx)
{
}

Proactor::impl::Connector::~Connector()
{
    close();
}

Proactor::impl::ConnectorPtr Proactor::impl::Connector::Create(Proactor::impl *pro
    , ConnectContextPtr ctx
    , ACE_INET_Addr remote
    , ACE_INET_Addr local)
{
    ConnectorPtr cp(new Connector(pro, ctx));
    cp->thisPtr_ = cp;
    cp->open(true /* pass_addresses */
        , &pro->ace_pro_ /* proactor */
        , true /* validate_new_connection */);
    cp->connect(remote, local);
    return cp;
}

int Proactor::impl::Connector::connect(const ACE_INET_Addr &remote_sap
                , const ACE_INET_Addr &local_sap
                , int reuse_addr
                , const void *act)
{
    ConnectorPtr strong = thisPtr_.lock();
    int rval;
    if((rval = Super::connect(remote_sap, local_sap, reuse_addr, act)) == 0) {
        pro_->registerPending(strong);
    }
    return rval;
}

void Proactor::impl::Connector::handle_connect(const ACE_Asynch_Connect::Result &result)
{
    ON_BLOCK_EXIT(boost::bind(&Proactor::impl::unregisterPending, pro_, thisPtr_.lock()));
    Super::handle_connect(result);
    ConnectResult cr;
    convert(cr, result);
    ctx_->sigComplete_(cr);
}

int Proactor::impl::Connector::validate_connection(const ACE_Asynch_Connect::Result &result
                            , const ACE_INET_Addr &remote
                            , const ACE_INET_Addr &local)
{
    ConnectResult cr;
    convert(cr, result);
    if (ctx_->sigValidate_(cr, remote, local)) {
        return 0;
    }
    return -1;
}

Proactor::impl::StreamHandler* Proactor::impl::Connector::make_handler()
{
    std::pair<StreamContextPtr, StreamHandlerPtr> stream = pro_->makeStream();
    ctx_->sigStream_(stream.first); // here stream should be set-up.
    return stream.second.get();
}

void Proactor::impl::Connector::close()
{
    cancel();
    if (this->handle() != ACE_INVALID_HANDLE) {
        ACE_HANDLE hval = handle();
        handle(ACE_INVALID_HANDLE);
        ACE_OS::closesocket(hval);
    }
}


} // namespace pyasynchio
