/* 
 *  \file buf.hpp
 *  \brief General-purpose buffer type declaration.
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_BUF_HPP_INCLUDED_
#define PYASYNCHIO_BUF_HPP_INCLUDED_

#pragma once

#include <boost/dynamic_bitset.hpp>
#include <algorithm>
#include <deque>
#include <stdexcept>

namespace pyasynchio {

//! general-purpose buffer.
class buf : public std::deque<char>
{
public:
    //! Parent typedef
    typedef std::deque<char> Super;
    //! Default constructor
    buf() {}
    //! Constructor borrowed from deque.
    explicit buf(size_t count) : Super(count) {}
    //! Constructor borrowed from deque.
    buf(size_t count, const char &val) : Super(count, val) {}

    //! Constructor from string, for convenience.
    buf(std::string value)
    {
        std::copy(value.begin(), value.end(), back_inserter(*this));
    }

    template<class Allocator>
    buf(const boost::dynamic_bitset<unsigned char, Allocator> &bs)
    {
        to_block_range(bs, back_inserter(*this));
    }


    //! Constructor from C-style string, for convenience.
    buf(char* value)
    {
        this->append(std::string(value));
    }
    
    buf(char* value, size_t length)
    {
        size_t idx;
        for(idx = 0; idx < length; ++idx) {
            push_back(value[idx]);
        }
    }

    //! appends parameter to this
    /*! @param what buf to append.
    */
    void append(const buf &what)
    {
        std::copy(what.begin(), what.end(), back_inserter(*this));
    }

    //! returns index of char value after given index. 
    /*! Throws std::invalid_argument if not found.
    @param what Value to search.
    @param from Search start position.
    @return index of first occurence of what.
    */
    size_t index(char what, size_t from = 0) const
    {
        buf::const_iterator end_citer = this->end();
        buf::const_iterator pos = find(this->begin() + from, end_citer, what);
        if (pos == end_citer) {
            throw std::invalid_argument("element not found");
        }
        return static_cast<size_t>(pos - this->begin());
    }

    //! slicing support
    /*! @param from Starting index.
    @param to Ending index.
    @return Slicing result.
    */
    buf slice(size_t from, size_t to) const
    {
        buf sl;
        if (from > to || to > size()) {
            throw std::invalid_argument("invalid slice index");
        }
        copy(this->begin() + from, this->begin() + to, back_inserter(sl));
        return sl;
    }

    //! converts to string
    /*! If 0 is encountered, throws runtime_error.
    */
    std::string asString() const
    {
        std::string value;
        buf::const_iterator citer, end_citer(this->end());
        for(citer = this->begin(); citer != end_citer; ++citer) {
            if (*citer != 0) {
                value.push_back(*citer);
            }
            else {
                throw std::runtime_error("zero encountered!");
            }
        }
        return value;

    }

    template<typename CharType>
    static buf FromHex(const CharType * s)
    {
        return FromHex(std::basic_string<CharType>(s));
    }


    template<typename CharType>
    static buf FromHex(const std::basic_string<CharType> hex)
    {
        if (hex.length() % 2 > 0) {
            assert(false);
            throw std::invalid_argument("hex string is invalid");
        }
        size_t j, k;
        buf b;
        b.resize(hex.length() / 2);
        for(j = 0; j < b.size(); ++j) {
            b[j] = 0;
            for (k = 0; k < 2; ++k) {
                unsigned char p;
                if (hex[2*j + k] >= '0' && hex[2*j + k] <= '9') {
                    p = hex[2*j + k] - '0';
                }
                else if (hex[2*j + k] >= 'a' && hex[2*j + k] <= 'f') {
                    p = hex[2*j + k] - 'a' + 0xA;
                }
                else if (hex[2*j + k] >= 'A' && hex[2*j + k] <= 'F') {
                    p = hex[2*j + k] - 'A' + 0xA;
                }
                else {
                    assert(false);
                    throw std::invalid_argument("digest hex is invalid");
                }
                b[j] |= (p << (k ? 0 : 4));
            }
        }
        return b;
    }

};


} // namespace pyasynchio

#endif // PYASYNCHIO_BUF_HPP_INCLUDED_
