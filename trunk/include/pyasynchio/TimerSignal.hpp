#ifndef SPACE_TRANSPORT_TIMERSIGNAL_HPP_INCLUDED_
#define SPACE_TRANSPORT_TIMERSIGNAL_HPP_INCLUDED_

#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

class ACE_Time_Value;

namespace space {

namespace transport {

class TimerSignal;
typedef boost::shared_ptr<TimerSignal> TimerSignalPtr;
typedef boost::signal1<void, ACE_Time_Value> TimerSignalBase;

class TimerSignal : public TimerSignalBase
{
public:
	static TimerSignalPtr Create()
	{
		return TimerSignalPtr(new TimerSignal());
	}
protected:
	TimerSignal()
	: TimerSignalBase()
	{}
};

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_TIMERSIGNAL_HPP_INCLUDED_