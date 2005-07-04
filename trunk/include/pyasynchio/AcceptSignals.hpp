/*!
 *  \file AcceptSignals.hpp
 *  \brief Type declarations for "accept" signals.
 *  \author Vladimir Sukhoy
 */

#ifndef SPACE_TRANSPORT_ACCEPTSIGNALS_HPP_INCLUDED_
#define SPACE_TRANSPORT_ACCEPTSIGNALS_HPP_INCLUDED_

#pragma once

#include <space/transport/StreamContext.hpp>
#include <space/transport/AcceptResult.hpp>
#include <boost/signals.hpp>
#include <space/transport/conditional_and_combiner.hpp>

namespace space {

namespace transport {

typedef boost::signal3<bool
    , const AcceptResult&
    , ACE_INET_Addr /* remote*/
    , ACE_INET_Addr /* local */
    , conditional_and_combiner<bool> > ValidateAcceptSignal;

typedef boost::signal1<void, const AcceptResult&> AcceptCompleteSignal;
typedef boost::signal1<void, StreamContextPtr> AcceptStreamSignal;


} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_ACCEPTSIGNALS_HPP_INCLUDED_