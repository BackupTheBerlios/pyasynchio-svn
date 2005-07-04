/*!
 *	\file Proactor_impl_StreamHandler.hpp
 *	\brief Proactor::impl::StreamHandler interface.
 *	\author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_PROACTOR_IMPL_STREAMHANDLER_HPP_INCLUDED_
#define SPACE_TRANSPORT_PROACTOR_IMPL_STREAMHANDLER_HPP_INCLUDED_

#pragma once

#include <space/transport/Proactor_impl.hpp>
#include <ace/Asynch_IO.h>

namespace space {

namespace transport {

class Proactor::impl::StreamHandler 
: public ACE_Handler
{
public:
	StreamHandler() 
	{
		assert(false); // shouldnt be called by any means.
		throw std::runtime_error("forbidden constructor");
	}
	
	virtual ~StreamHandler();
	
	void act(const void *act);
	void addresses(ACE_INET_Addr remote, ACE_INET_Addr local);
	void open(ACE_HANDLE new_handle, ACE_Message_Block &message_block);
	
	static StreamHandlerPtr Create(Proactor::impl *pro, StreamContextPtr ctx);
		
	void read(size_t count, const void *act, int priority, int signal);
	void write(const buf &data, const void *act, int priority, int signal);
	
	void cancelRead();
	void cancelWrite();
	
	void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);
	void handle_write_stream(const ACE_Asynch_Write_Stream::Result &result);
	
	void close();
protected:
	StreamHandler(Proactor::impl *pro, StreamContextPtr ctx);
	
private:
	Proactor::impl *pro_;
	StreamContextPtr ctx_;
	StreamHandlerWeakPtr thisPtr_;
	ACE_Asynch_Read_Stream reader_;
	ACE_Asynch_Write_Stream writer_;
};

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_PROACTOR_IMPL_STREAMHANDLER_HPP_INCLUDED_