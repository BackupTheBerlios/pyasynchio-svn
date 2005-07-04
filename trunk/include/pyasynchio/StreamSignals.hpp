/*!
 *  \file StreamSignals.hpp
 *  \brief Type declarations for "stream" signals.
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_STREAMSIGNALS_HPP_INCLUDED_
#define SPACE_TRANSPORT_STREAMSIGNALS_HPP_INCLUDED_

#pragma once

#include <boost/signals.hpp>
#include <space/transport/StreamResults.hpp>

class ACE_INET_Addr;

namespace space {

namespace transport {

typedef boost::signal2<void, ACE_INET_Addr, ACE_INET_Addr> AddressesSignal;
typedef boost::signal1<void, const void*> ActSignal;
typedef boost::signal1<void, void*> OpenSignal;

typedef boost::signal1<void, const ReadStreamResult &> ReadStreamSignal;
typedef boost::signal1<void, const WriteStreamResult &> WriteStreamSignal;

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_STREAMSIGNALS_HPP_INCLUDED_