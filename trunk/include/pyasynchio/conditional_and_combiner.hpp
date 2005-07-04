#ifndef SPACE_TRANSPORT_CONDITIONAL_AND_COMBINER_HPP_INCLUDED_
#define SPACE_TRANSPORT_CONDITIONAL_AND_COMBINER_HPP_INCLUDED_

#pragma once

namespace space {

namespace transport {

template<typename T>
struct conditional_and_combiner
{
    typedef T result_type;
    
    template<typename InputIterator>
    T operator ()(InputIterator first, InputIterator last) const
    {
        while(first != last) {
            if(!(*first)) {
                return false;
            }
            ++first;
        }
        
        return true;
    }
};

} // namespace transport

} // namespace space

#endif // SPACE_TRANSPORT_CONDITIONAL_AND_COMBINER_HPP_INCLUDED_