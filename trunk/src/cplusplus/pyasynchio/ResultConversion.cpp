/*  \file ResultConversion.cpp
 *  \brief Implementation of result conversion routines from ace 
 *  to pyasynchio.
 *  \author Vladimir Sukhoy
 */

#include <pyasynchio/ResultConversion.hpp>
#include <ace/Message_Block.h>

namespace pyasynchio {

Result& convert(Result &lhs
                , const ACE_Asynch_Result &rhs)
{
    lhs.act_ = rhs.act();
    lhs.success_ = rhs.success();
    lhs.ckey_ = rhs.completion_key();
    lhs.error_ = rhs.error();
    lhs.event_ = rhs.event();
    lhs.priority_ = rhs.priority();
    lhs.signo_ = rhs.signal_number();
    lhs.offset_ = (static_cast<integer>(rhs.offset_high()) << 8*sizeof(unsigned long))
        & rhs.offset();
    return lhs;
}

AcceptResult& convert(AcceptResult &lhs
                      , const ACE_Asynch_Accept::Result &rhs)
{
    convert(static_cast<Result&>(lhs), rhs);

    lhs.listen_handle_ = rhs.listen_handle();
    lhs.accept_handle_ = rhs.accept_handle();
    lhs.message_block_ = lhs.FromFunctor(
        boost::bind(&ACE_Message_Block::rd_ptr, &rhs.message_block())
        , rhs.bytes_to_read());

    return lhs;
}


ConnectResult& convert(ConnectResult &lhs
                       , const ACE_Asynch_Connect::Result &rhs)
{
    convert(static_cast<Result&>(lhs), rhs);
    lhs.conn_handle_ = rhs.connect_handle();
    return lhs;
}

ReadStreamResult& convert(ReadStreamResult &lhs
                          , const ACE_Asynch_Read_Stream::Result &rhs)
{
    convert(static_cast<Result&>(lhs), rhs);
    lhs.handle_ = rhs.handle();
    lhs.data_ = lhs.FromFunctor(
        boost::bind(&ACE_Message_Block::rd_ptr, &rhs.message_block())
        , rhs.message_block().length());
    return lhs;
}

WriteStreamResult& convert(WriteStreamResult &lhs
                           , const ACE_Asynch_Write_Stream::Result &rhs)
{
    convert(static_cast<Result&>(lhs), rhs);
    lhs.handle_ = rhs.handle();
    lhs.data_ = lhs.FromFunctor(
        boost::bind(&ACE_Message_Block::rd_ptr, &rhs.message_block())
        , rhs.message_block().length());
    return lhs;
}

} // namespace pyasynchio
