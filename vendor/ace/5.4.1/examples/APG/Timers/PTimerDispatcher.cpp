// PTimerDispatcher.cpp,v 1.2 2004/01/08 17:08:00 shuston Exp

#include "PTimerDispatcher.h"

void PTimer_Dispatcher::wait_for_event (void)
{
  ACE_TRACE (ACE_TEXT ("PTimer_Dispatcher::wait_for_event"));

  while (1)
    {
      ACE_Time_Value max_tv = timer_queue_->gettimeofday ();

      ACE_Time_Value *this_timeout =
        this->timer_queue_->calculate_timeout (&max_tv);

      if (*this_timeout == ACE_Time_Value::zero)
        this->timer_queue_->expire ();
      else
        {
          // Convert to absolute time.
          ACE_Time_Value next_timeout =
            timer_queue_->gettimeofday ();
          next_timeout += *this_timeout;
          if (this->timer_.wait (&next_timeout) == -1 )
            this->timer_queue_->expire ();
        }
    }
}

long
PTimer_Dispatcher::schedule (PCB *cb,
                             void *arg,
                             const ACE_Time_Value &abs_time,
                             const ACE_Time_Value &interval)
{
  ACE_TRACE (ACE_TEXT ("PTimer_Dispatcher::schedule_timer"));

  return this->timer_queue_->schedule
    (cb, arg, abs_time, interval);
}

int
PTimer_Dispatcher::cancel (PCB *cb,
                           int dont_call_handle_close)
{
  ACE_TRACE (ACE_TEXT ("PTimer_Dispatcher::cancel"));
  return timer_queue_->cancel (cb, dont_call_handle_close);
}

void PTimer_Dispatcher::set (PTimerQueue *timer_queue)
{
  ACE_TRACE (ACE_TEXT ("PTimer_Dispatcher::set"));

  timer_queue_ = timer_queue;
}

int
PTimer_Dispatcher::reset_interval (long timer_id,
                                   const ACE_Time_Value &interval)
{
  ACE_TRACE (ACE_TEXT ("PTimer_Dispatcher::reset_interval"));

  return timer_queue_->reset_interval (timer_id, interval);
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Singleton <PTimer_Dispatcher, ACE_Null_Mutex>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Singleton <PTimer_Dispatcher, ACE_Null_Mutex>
#elif defined (__GNUC__) && (defined (_AIX) || defined (__hpux))
template ACE_Singleton<PTimer_Dispatcher, ACE_Null_Mutex> *
      ACE_Singleton<PTimer_Dispatcher, ACE_Null_Mutex>::singleton_;
# endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
