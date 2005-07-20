/*!
 *  \file Proactor_impl_StreamHandler.cpp
 *  \brief Implementation for pyasynchio::Proactor::impl::StreamHandler.
 *  \author Vladimir Sukhoy
 */

#include <pyasynchio/detail/fix_ace_warnings.hpp>
#include <ace/INET_Addr.h>
#include <ace/OS.h>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/bind.hpp>
#include <pyasynchio/Proactor_impl_StreamHandler.hpp>
#include <pyasynchio/ResultConversion.hpp>
#include <pyasynchio/scopeguard.hpp>

namespace pyasynchio {

Proactor::impl::StreamHandler::StreamHandler(Proactor::impl *pro
	, AbstractStreamHandlerPtr user_stream_handler)
: pro_(pro)
, user_stream_handler_(user_stream_handler)
{
}

Proactor::impl::StreamHandlerPtr Proactor::impl::StreamHandler::Create(
    Proactor::impl *pro
	, AbstractStreamHandlerPtr user_stream_handler)
{
    StreamHandlerPtr shp(new StreamHandler(pro, user_stream_handler));
    shp->thisPtr_ = shp;
    return shp;
}


Proactor::impl::StreamHandler::~StreamHandler()
{
    close();
}

void Proactor::impl::StreamHandler::act(const void *act)
{
    user_stream_handler_->notify_act(act);
}

void Proactor::impl::StreamHandler::addresses(ACE_INET_Addr remote, ACE_INET_Addr local)
{
	user_stream_handler_->notify_endpoints(remote, local);
}

void Proactor::impl::StreamHandler::open(ACE_HANDLE new_handle, ACE_Message_Block &message_block)
{
    reader_.open(*this);
    writer_.open(*this);
	user_stream_handler_->notify_opened(new_handle);
	user_stream_handler_->notify_initialized();
}

void Proactor::impl::StreamHandler::read(size_t count
    , const void *act
    , int priority
    , int signal)
{
    pro_->registerPending(thisPtr_.lock());
    ACE_Message_Block *mb = new ACE_Message_Block(count);
    reader_.read(*mb, count, act, priority, signal);
}

void Proactor::impl::StreamHandler::cancelRead()
{
    reader_.cancel();
}

void Proactor::impl::StreamHandler::write(const buf &data
                                        , const void *act
                                        , int priority
                                        , int signal)
{
    pro_->registerPending(thisPtr_.lock());
    ACE_Message_Block *mb = new ACE_Message_Block(data.size());

    buf::const_iterator bci, bend(data.end());
    char *p = mb->wr_ptr();
    for (bci = data.begin(); bci != bend; ++bci) {
        (*p++) = *bci;
    }
    mb->length(data.size());
    
    writer_.write(*mb, data.size(), act, priority, signal);
}

void Proactor::impl::StreamHandler::cancelWrite()
{
    writer_.cancel();
}

void Proactor::impl::StreamHandler::handle_read_stream(
                                        const ACE_Asynch_Read_Stream::Result &result)
{
    ON_BLOCK_EXIT(boost::bind(&Proactor::impl::unregisterPending, pro_, thisPtr_.lock()));
    ON_BLOCK_EXIT(boost::lambda::bind(boost::lambda::delete_ptr(), &result.message_block()));
    ReadStreamResult r;
    convert(r, result);
	user_stream_handler_->read_completed(r);
}

void Proactor::impl::StreamHandler::handle_write_stream(
                                        const ACE_Asynch_Write_Stream::Result &result)
{
    ON_BLOCK_EXIT(boost::bind(&Proactor::impl::unregisterPending, pro_, thisPtr_.lock()));
    ON_BLOCK_EXIT(boost::lambda::bind(boost::lambda::delete_ptr(), &result.message_block()));
    WriteStreamResult r;
    convert(r, result);
	user_stream_handler_->write_completed(r);
}

void Proactor::impl::StreamHandler::close()
{
    cancelRead();
    cancelWrite();
    if (this->handle() != ACE_INVALID_HANDLE) {
        ACE_HANDLE hval = handle();
        handle(ACE_INVALID_HANDLE);
        ACE_OS::closesocket(hval);
    }
}

} // namespace pyasynchio
