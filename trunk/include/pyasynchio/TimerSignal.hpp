#ifndef PYASYNCHIO_TIMERSIGNAL_HPP_INCLUDED_
#define PYASYNCHIO_TIMERSIGNAL_HPP_INCLUDED_

#pragma once

#include <boost/signal.hpp>
#include <boost/shared_ptr.hpp>

class ACE_Time_Value;

namespace pyasynchio {

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

} // namespace pyasynchio

#endif // PYASYNCHIO_TIMERSIGNAL_HPP_INCLUDED_