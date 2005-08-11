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


} // namespace pyasynchio

#endif // PYASYNCHIO_SOCKETMODULE_STUFF_HPP_INCLUDED_