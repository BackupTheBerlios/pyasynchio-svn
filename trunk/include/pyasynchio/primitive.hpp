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
