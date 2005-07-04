/*!
 *  \file ConnectSignals.hpp
 *  \brief Type declarations for "connect" signals.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_CONNECTSIGNALS_HPP_INCLUDED_
#define PYASYNCHIO_CONNECTSIGNALS_HPP_INCLUDED_

#pragma once

#include <pyasynchio/StreamContext.hpp>
#include <pyasynchio/ConnectResult.hpp>
#include <pyasynchio/conditional_and_combiner.hpp>
#include <boost/signals.hpp>

namespace pyasynchio {

typedef boost::signal3<bool
    , const ConnectResult&
    , ACE_INET_Addr /* remote*/
    , ACE_INET_Addr /* local */
    , conditional_and_combiner<bool> > ValidateConnectSignal;

typedef boost::signal1<void, const ConnectResult&> ConnectCompleteSignal;
typedef boost::signal1<void, StreamContextPtr> ConnectStreamSignal;

} // namespace pyasynchio

#endif // PYASYNCHIO_CONNECTSIGNALS_HPP_INCLUDED_