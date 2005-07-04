/*!
 *  \file StreamSignals.hpp
 *  \brief Type declarations for "stream" signals.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_STREAMSIGNALS_HPP_INCLUDED_
#define PYASYNCHIO_STREAMSIGNALS_HPP_INCLUDED_

#pragma once

#include <boost/signals.hpp>
#include <pyasynchio/StreamResults.hpp>

class ACE_INET_Addr;

namespace pyasynchio {

typedef boost::signal2<void, ACE_INET_Addr, ACE_INET_Addr> AddressesSignal;
typedef boost::signal1<void, const void*> ActSignal;
typedef boost::signal1<void, void*> OpenSignal;

typedef boost::signal1<void, const ReadStreamResult &> ReadStreamSignal;
typedef boost::signal1<void, const WriteStreamResult &> WriteStreamSignal;

} // namespace pyasynchio

#endif // PYASYNCHIO_STREAMSIGNALS_HPP_INCLUDED_