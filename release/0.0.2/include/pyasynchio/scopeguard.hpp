#ifndef PYASYNCHIO_SCOPEGUARD_HPP_INCLUDED_
#define PYASYNCHIO_SCOPEGUARD_HPP_INCLUDED_

#include <boost/function/function0.hpp> 
#include <boost/config/suffix.hpp>
#include <boost/utility.hpp>

class scope_guard : private boost::noncopyable
{
public:
    explicit scope_guard( const boost::function0<void> & v )
        :   f_(v)
        ,   dismissed_(false) 
    {
    }

    ~scope_guard() //throw()
    {
        if(!dismissed_)
        {
            f_();
        }
    }

    void dismiss() //throw() 
    { 
        dismissed_ = true; 
    }

private:
    bool dismissed_;
    const boost::function0<void>  f_;

    // Disable copy and assignment - not implemented.
    // Declaring them here keeps VC6 from complaining that it can't 
    // generate 'em
    scope_guard& operator=(const scope_guard&);
    scope_guard(const scope_guard&);
};

#ifndef BOOST_MSVC
#define ANONYMOUS_VARIABLE(str) BOOST_JOIN(str, __LINE__)
#else
#define ANONYMOUS_VARIABLE(str) BOOST_JOIN(str, __COUNTER__)
#endif
#define ON_BLOCK_EXIT(F) scope_guard     ANONYMOUS_VARIABLE(anonymousScopeGuard)(F) 

#endif //PYASYNCHIO_SCOPEGUARD_HPP_INCLUDED_