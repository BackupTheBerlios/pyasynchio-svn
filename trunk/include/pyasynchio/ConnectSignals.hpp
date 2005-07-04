/*!
 *  \file ConnectSignals.hpp
 *  \brief Type declarations for "connect" signals.
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_CONNECTSIGNALS_HPP_INCLUDED_
#define SPACE_TRANSPORT_CONNECTSIGNALS_HPP_INCLUDED_

#pragma once

#include <space/transport/StreamContext.hpp>
#include <space/transport/ConnectResult.hpp>
#include <space/transport/conditional_and_combiner.hpp>
#include <boost/signals.hpp>

namespace space {

namespace transport {

typedef boost::signal3<bool
    , const ConnectResult&
    , ACE_INET_Addr /* remote*/
    , ACE_INET_Addr /* local */
    , conditional_and_combiner<bool> > ValidateConnectSignal;

typedef boost::signal1<void, const ConnectResult&> ConnectCompleteSignal;
typedef boost::signal1<void, StreamContextPtr> ConnectStreamSignal;



} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_CONNECTSIGNALS_HPP_INCLUDED_