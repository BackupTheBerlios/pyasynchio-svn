/*
Copyright (c) 2005 Vladimir Sukhoy

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is furnished 
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


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