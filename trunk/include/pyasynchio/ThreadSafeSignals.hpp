/*!
 *  \file ThreadSafeSignals.hpp
 *
 *  \brief Exploratory attempt to wrap boost::signals in order to achieve
 *  thread safety.
 *
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_THREADSAFESIGNALS_HPP_INCLUDED_
#define PYASYNCHIO_THREADSAFESIGNALS_HPP_INCLUDED_

#pragma once

#include <boost/signal.hpp>
#include <boost/thread/read_write_mutex.hpp>
#include <boost/type_traits/function_traits.hpp>

namespace pyasynchio {

class thread_safe_signal_base
{
public:
    typedef boost::read_write_mutex mutex_t;
    typedef mutex_t::scoped_read_lock scoped_read_lock_t;
    typedef mutex_t::scoped_write_lock scoped_write_lock_t;
protected:
    mutex_t mut_;

    thread_safe_signal_base()
        : mut_(boost::read_write_scheduling_policy::writer_priority)
    {}

};


template<class signal, class Signature>
class signal_wrapper_base 
{
protected:
    signal *sig_;
    typedef boost::function_traits<Signature> s;
public:
    signal_wrapper_base(signal *sig) : sig_(sig) {}
};

template<int arity, class signal, class Signature> class signal_wrapper;

// here goes some bullshit for functor support.
#include "signal_wrapper0.hpp"
#include "signal_wrapper1.hpp"
#include "signal_wrapper2.hpp"
#include "signal_wrapper3.hpp"
#include "signal_wrapper4.hpp"

/*!
 *  @brief Template wrapper around boost::signal, which owns builtin 
 *  readers/writer mutex.
 */
template<typename Signature
    , typename Combiner = boost::last_value<typename boost::function_traits<Signature>::result_type>
    , typename Group = int
    , typename GroupCompare = std::less<Group>
    , typename SlotFunction = boost::function<Signature> >
class thread_safe_signal 
    : public signal_wrapper<boost::function_traits<Signature>::arity
        , boost::signal<Signature, Combiner, Group, GroupCompare, SlotFunction>
        , Signature>
    , public boost::signal<Signature, Combiner, Group, GroupCompare, SlotFunction>
{
private:
public:
    typedef boost::signal<Signature, Combiner, Group, GroupCompare, SlotFunction> parent_t;
    typedef signal_wrapper<boost::function_traits<Signature>::arity
        , boost::signal<Signature, Combiner, Group, GroupCompare, SlotFunction> 
        , Signature> selector_t;

    thread_safe_signal(const typename parent_t::combiner_type &comb = parent_t::combiner_type(),
                       const typename parent_t::group_compare_type &gcomp = parent_t::group_compare_type())
    : parent_t(comb, gcomp)
#pragma warning(disable:4355)
    , selector_t(this)
#pragma warning(default:4355)
    {}

    ~thread_safe_signal()
    {
        thread_safe_signal_base::scoped_write_lock_t srl(mut_);
    }

    // connection management
    boost::BOOST_SIGNALS_NAMESPACE::connection 
    connect(const typename parent_t::slot_type &st
            , boost::BOOST_SIGNALS_NAMESPACE::connect_position cp = boost::BOOST_SIGNALS_NAMESPACE::at_back)
    {
        thread_safe_signal_base::scoped_write_lock_t swl(mut_);
        return parent_t::connect(st, cp);
    }
    boost::BOOST_SIGNALS_NAMESPACE::connection 
    connect(const typename parent_t::group_type &gt
            , const typename parent_t::slot_type &st
            , boost::BOOST_SIGNALS_NAMESPACE::connect_position cp = boost::BOOST_SIGNALS_NAMESPACE::at_back)
    {
        thread_safe_signal_base::scoped_write_lock_t swl(mut_);
        return parent_t::connect(gt, st, cp);
    }
    
    void disconnect(const typename parent_t::group_type &gt)
    {
        thread_safe_signal_base::scoped_write_lock_t swl(mut_);
        parent_t::disconnect(gt);
    }
    template<typename Slot> void disconnect(const Slot& sl)
    {
        thread_safe_signal_base::scoped_write_lock_t swl(mut_);
        parent_t::disconnect(sl);
    }
    void disconnect_all_slots()
    {
        thread_safe_signal_base::scoped_write_lock_t swl(mut_);
        parent_t::disconnect_all_slots();
    }
    bool empty() const
    {
        thread_safe_signal_base::scoped_read_lock_t srl(mut_);
        return parent_t::empty();
    }
    std::size_t num_slots() const
    {
        thread_safe_signal_base::scoped_read_lock_t srl(mut_);
        return parent_t::num_slots();
    }

    // combiner access
    typename parent_t::combiner_type& combiner()
    {
        thread_safe_signal_base::scoped_write_lock_t swl(mut_);
        return parent_t::combiner();
    }
    const typename parent_t::combiner_type& combiner() const
    {
        thread_safe_signal_base::scoped_read_lock_t srl(mut_);
        return parent_t::combiner();
    }

};

} // namespace pyasynchio

#endif // PYASYNCHIO_THREADSAFESIGNAL_HPP_INCLUDED_