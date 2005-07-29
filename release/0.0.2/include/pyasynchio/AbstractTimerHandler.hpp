#ifndef PYASYNCHIO_ABSTRACT_TIMER_HANDLER_HPP_INCLUDED_
#define PYASYNCHIO_ABSTRACT_TIMER_HANDLER_HPP_INCLUDED_

#pragma once

#include <boost/shared_ptr.hpp>

class ACE_Time_Value;

namespace pyasynchio {

class AbstractTimerHandler;
typedef boost::shared_ptr<AbstractTimerHandler> AbstractTimerHandlerPtr;

class AbstractTimerHandler
{
public:
    AbstractTimerHandler() {}
    virtual ~AbstractTimerHandler() {}

    virtual void notify_elapsed(const ACE_Time_Value &ct) = 0;
};

} // namespace pyasynchio

#endif // PYASYNCHIO_ABSTRACT_TIMER_HANDLER_HPP_INCLUDED_