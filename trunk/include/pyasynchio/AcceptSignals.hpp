/*!
 *  \file AcceptSignals.hpp
 *  \brief Type declarations for "accept" signals.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_ACCEPTSIGNALS_HPP_INCLUDED_
#define PYASYNCHIO_ACCEPTSIGNALS_HPP_INCLUDED_

#pragma once

#include <pyasynchio/StreamContext.hpp>
#include <pyasynchio/AcceptResult.hpp>
#include <boost/signals.hpp>
#include <pyasynchio/conditional_and_combiner.hpp>

namespace pyasynchio {

typedef boost::signal3<bool
    , const AcceptResult&
    , ACE_INET_Addr /* remote*/
    , ACE_INET_Addr /* local */
    , conditional_and_combiner<bool> > ValidateAcceptSignal;

typedef boost::signal1<void, const AcceptResult&> AcceptCompleteSignal;
typedef boost::signal1<void, StreamContextPtr> AcceptStreamSignal;

} // namespace pyasynchio

#endif // PYASYNCHIO_ACCEPTSIGNALS_HPP_INCLUDED_