/*!
 *  \file StreamContext.hpp
 *  \brief Signal context for stream operations.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_STREAMCONTEXT_HPP_INCLUDED_
#define PYASYNCHIO_STREAMCONTEXT_HPP_INCLUDED_

#pragma once

#include <pyasynchio/StreamSignals.hpp>
#include <boost/shared_ptr.hpp>

namespace pyasynchio {

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

} // namespace pyasynchio

#endif // PYASYNCHIO_STREAMCONTEXT_HPP_INCLUDED_