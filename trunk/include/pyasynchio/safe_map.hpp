/*! \file safe_map.hpp
    \brief safer map adaptation.
    \author Vladimir Sukhoy
*/

#ifndef PYASYNCHIO_SAFE_MAP_HPP_INCLUDED_
#define PYASYNCHIO_SAFE_MAP_HPP_INCLUDED_

#pragma once

#include <map>
#include <stdexcept>

namespace pyasynchio {

//! safe map template. 
/*! Operator [] throws if argument is not found.
    "insert" for new values, and [] only for extraction which throws.
*/
template < 
    class Key,
    class Type,
    class Traits = std::less<Key>,
    class Allocator = std::allocator< std::pair<const Key, Type> >
>
class safe_map : public std::map<Key, Type, Traits, Allocator>
{
public:
    typedef std::map<Key, Type, Traits, Allocator> Super;
    safe_map()
        : Super()
    {}
    explicit safe_map(const Traits& _Comp)
        : Super(_Comp)
    {}
    explicit safe_map(const Traits& _Comp, const Allocator& _Al)
        : Super(_Comp, _Al)
    {}
    safe_map(const Super& _Right)
        : Super(_Right)
    {}
    template<class InputIterator>
    safe_map(
        InputIterator _First,
        InputIterator _Last
    )
    : Super(_First, _Last)
    {}
    template<class InputIterator>
    safe_map(
        InputIterator _First,
        InputIterator _Last,
        const Traits& _Comp
    )
    : Super(_First, _Last, _Comp)
    {}

    template<class InputIterator>
    safe_map(
        InputIterator _First,
        InputIterator _Last,
        const Traits& _Comp,
        const Allocator& _Al
    )
    : Super(_First, _Last, _Comp, _Al)
    {}

    //! const operator in safe_map.
    /*! It can be used only for extraction of values, and throws
        std::invalid_argument exception if key is not found in map.
    */
    const Type& operator[](const Key &key) const
    {
        const_iterator citer = this->find(key);
        if (citer == this->end()) {
            throw std::invalid_argument("key not found");
        }
        return citer->second;
    }

    //! non-const operator in safe_map.
    /*! @see const Type& safe_map::operator[] const
    */
    Type& operator[](const Key &key)
    {
        iterator iter = this->find(key);
        if (iter == this->end()) {
            throw std::invalid_argument("key not found");
        }
        return iter->second;
    }

};

} // namespace pyasynchio

#endif //SAFE_MAP_HPP_INCLUDED_