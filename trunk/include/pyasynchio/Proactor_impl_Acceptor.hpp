/*!
 *	\file Proactor_impl_Acceptor.hpp
 *	\brief Proactor::impl::Acceptor interface.
 *	\author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_PROACTOR_IMPL_ACCEPTOR_HPP_INCLUDED_
#define SPACE_TRANSPORT_PROACTOR_IMPL_ACCEPTOR_HPP_INCLUDED_

#pragma once
 
#include <space/transport/Proactor_impl.hpp>
#include <space/transport/Proactor_impl_StreamHandler.hpp>
#include <ace/Asynch_Acceptor.h>
#include <boost/signals/trackable.hpp>

namespace space {

namespace transport {

class Proactor::impl::Acceptor 
: public ACE_Asynch_Acceptor<StreamHandler>
, public boost::signals::trackable
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
		, AcceptContextPtr ctx
		, ACE_INET_Addr addr);

protected:
	Acceptor(Proactor::impl *pro
		, AcceptContextPtr ctx
		, ACE_INET_Addr addr);

private:
	Proactor::impl *pro_;
	AcceptContextPtr ctx_;
	AcceptorWeakPtr thisPtr_;
};


} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_PROACTOR_IMPL_ACCEPTOR_HPP_INCLUDED_