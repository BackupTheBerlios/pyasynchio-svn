/*!
 *	\file Proactor_impl_Connector.hpp
 *	\brief Proactor::impl::Connector interface.
 *	\author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_PROACTOR_IMPL_CONNECTOR_HPP_INCLUDED_
#define SPACE_TRANSPORT_PROACTOR_IMPL_CONNECTOR_HPP_INCLUDED_

#pragma once

#include <space/transport/Proactor_impl.hpp>
#include <space/transport/Proactor_impl_StreamHandler.hpp>
#include <ace/Asynch_Connector.h>
#include <boost/signals/trackable.hpp>

namespace space {

namespace transport {

class Proactor::impl::Connector
: public ACE_Asynch_Connector<StreamHandler>
, public boost::signals::trackable
{
public:
	typedef ACE_Asynch_Connector<StreamHandler> Super;
	virtual ~Connector();

	int validate_connection(const ACE_Asynch_Connect::Result &result
							, const ACE_INET_Addr &remote
							, const ACE_INET_Addr &local);

	int connect(const ACE_INET_Addr &remote_sap
				, const ACE_INET_Addr &local_sap
				, int reuse_addr = 1
				, const void *act = 0);
	void handle_connect(const ACE_Asynch_Connect::Result &result);

	void close();

	StreamHandler* make_handler();

	static ConnectorPtr Create(Proactor::impl *pro
								, ConnectContextPtr ctx
								, ACE_INET_Addr remote
								, ACE_INET_Addr local);

protected:
	Connector(Proactor::impl *pro
			, ConnectContextPtr ctx);

private:
	Proactor::impl *pro_;
	ConnectContextPtr ctx_;
	ConnectorWeakPtr thisPtr_;
};

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_PROACTOR_IMPL_CONNECTOR_HPP_INCLUDED_