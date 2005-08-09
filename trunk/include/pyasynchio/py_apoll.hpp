#ifndef PYASYNCHIO_PY_APOLL_HPP_INCLUDED_
#define PYASYNCHIO_PY_APOLL_HPP_INCLUDED_

#pragma once

#include <pyasynchio/detail/config.hpp>
#include <python.h>
#include "socketmodule.h"
#include "fileobject.h"
#include <windows.h>

namespace pyasynchio {

class PYASYNCHIO_LINK_DECL Py_apoll : public PyObject
{
    Py_apoll();
    ~Py_apoll();

    void accept(::PySocketSockObject *so, ::PyObject *act);
    void connect(::PySocketSockObject *so, ::PyObject *addro, ::PyObject *acto);
    void send(::PySocketSockObject *so, ::PyObject *datao, unsigned long flags
        , ::PyObject *acto);
    void sendto(::PySocketSockObject *so, ::PyObject *addro
        , ::PyObject *datao, ::PyObject *acto);
    void recv(::PySocketSockObject *so, unsigned long size, unsigned long flags
        , ::PyObject *acto);
    void cancel(::PyObject *o);
    void close(::PyObject *o);

	::PyObject* poll(unsigned long ms);

    HANDLE iocp_handle_;
};


extern "C" PYASYNCHIO_LINK_DECL void initpyasynchio(void);

} // namespace pyasynchio

#endif // PYASYNCHIO_PY_APOLL_HPP_INCLUDED_