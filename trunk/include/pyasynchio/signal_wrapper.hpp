/* \file signal_wrapper
 *  
 * \brief Designed for numerous includes, so no guards.
 *
 * \author Vladimir Sukhoy
 */


template<class signal, class Signature>
class signal_wrapper<PYASYNCHIO_BOOST_SIGNAL_WRAPPER_ARITY, signal, Signature>
    : public thread_safe_signal_base
    , public signal_wrapper_base<signal, Signature>
{
public:
    signal_wrapper(signal * sig) : signal_wrapper_base<signal, Signature>(sig) {}

    typename signal::result_type operator()(PYASYNCHIO_BOOST_SIGNAL_WRAPPER_TYPED_ARGUMENTS) 
    {
        scoped_write_lock_t swl(mut_);
        return (*sig_)(PYASYNCHIO_BOOST_SIGNAL_WRAPPER_ARGUMENTS); 
    }
    typename signal::result_type operator()(PYASYNCHIO_BOOST_SIGNAL_WRAPPER_TYPED_ARGUMENTS) const 
    {
        scoped_read_lock_t srl(mut_);
        return (*sig_)(PYASYNCHIO_BOOST_SIGNAL_WRAPPER_ARGUMENTS);
    }
};
