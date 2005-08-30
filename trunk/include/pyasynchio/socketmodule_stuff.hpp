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


#ifndef PYASYNCHIO_SOCKETMODULE_STUFF_HPP_INCLUDED_
#define PYASYNCHIO_SOCKETMODULE_STUFF_HPP_INCLUDED_

#pragma once

#include <pyasynchio/detail/config.hpp>
#pragma push_macro("_DEBUG")
#undef _DEBUG
#include <pyconfig.h>
#pragma pop_macro("_DEBUG")
#include <python.h>
#include "socketmodule.h"

namespace pyasynchio {

PYASYNCHIO_LINK_DECL PyObject * makeipaddr(sockaddr *addr, int addrlen);
PYASYNCHIO_LINK_DECL PyObject * makesockaddr(int sockfd, sockaddr *addr
                                                , int addrlen, int proto);
PYASYNCHIO_LINK_DECL int getsockaddrarg(PySocketSockObject *s, PyObject *args,
                                        struct sockaddr *addrp, int *len_ret);

} // namespace pyasynchio

#endif // PYASYNCHIO_SOCKETMODULE_STUFF_HPP_INCLUDED_