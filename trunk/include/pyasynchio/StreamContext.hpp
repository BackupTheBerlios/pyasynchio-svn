/*!
 *  \file StreamContext.hpp
 *  \brief Signal context for stream operations.
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_STREAMCONTEXT_HPP_INCLUDED_
#define SPACE_TRANSPORT_STREAMCONTEXT_HPP_INCLUDED_

#pragma once

#include <space/transport/StreamSignals.hpp>
#include <boost/shared_ptr.hpp>

namespace space {

namespace transport {

class StreamContext; 
typedef boost::shared_ptr<StreamContext> StreamContextPtr;
typedef boost::weak_ptr<StreamContext> StreamContextWeakPtr;

class StreamContext 
{
public:
	ActSignal sigAct_;
	AddressesSignal sigAddresses_;
	OpenSignal sigOpen_;
	ReadStreamSignal sigRead_;
	WriteStreamSignal sigWrite_;
	
	static StreamContextPtr Create() { return StreamContextPtr(new StreamContext); }
protected:
	StreamContext() {}
};



} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_STREAMCONTEXT_HPP_INCLUDED_