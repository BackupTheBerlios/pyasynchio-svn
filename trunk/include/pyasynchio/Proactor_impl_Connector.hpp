/*!
 *	\file Proactor_impl_Connector.hpp
 *	\brief Proactor::impl::Connector interface.
 *	\author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_PROACTOR_IMPL_CONNECTOR_HPP_INCLUDED_
#define PYASYNCHIO_PROACTOR_IMPL_CONNECTOR_HPP_INCLUDED_

#pragma once

#include <pyasynchio/Proactor_impl.hpp>
#include <pyasynchio/Proactor_impl_StreamHandler.hpp>
#include <ace/Asynch_Connector.h>
#include <boost/signals/trackable.hpp>

namespace pyasynchio {

class Proactor::impl::Connector
: public ACE_Asynch_Connector<Proactor::impl::StreamHandler>
, public boost::signals::trackable
{
public:
	typedef ACE_Asynch_Connector<Proactor::impl::StreamHandler> Super;
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
		, AbstractConnectHandlerPtr user_connect_handler
		, ACE_INET_Addr remote
		, ACE_INET_Addr local);

protected:
	Connector(Proactor::impl *pro
			, AbstractConnectHandlerPtr user_connect_handler);

private:
	Proactor::impl *pro_;
	AbstractConnectHandlerPtr user_connect_handler_;
	ConnectorWeakPtr thisPtr_;
};



} // namespace pyasynchio

#endif // PYASYNCHIO_PROACTOR_IMPL_CONNECTOR_HPP_INCLUDED_