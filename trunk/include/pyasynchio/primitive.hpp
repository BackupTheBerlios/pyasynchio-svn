/*! \file types.hpp
 *  \brief Common type declarations
 *  \author Vladimir Sukhoy
 */

#ifndef PYASYNCHIO_PRIMITIVE_HPP_INCLUDED_
#define PYASYNCHIO_PRIMITIVE_HPP_INCLUDED_

#pragma once

namespace pyasynchio {

typedef unsigned char   uchar;
typedef uchar byte;

typedef char   int8;
typedef unsigned char  uint8;

typedef short  int16;
typedef unsigned short uint16;

typedef int  int32;
typedef unsigned int uint32;

typedef __int64 int64;
typedef unsigned __int64 uint64;

//! Integer base for all calculations
/*! 64-bit integers are used currently. */
typedef __int64 integer;


} // namespace pyasynchio

#endif // PYASYNCHIO_PRIMITIVE_HPP_INCLUDED_
