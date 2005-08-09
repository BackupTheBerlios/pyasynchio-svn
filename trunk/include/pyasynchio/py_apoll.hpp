#ifndef PYASYNCHIO_PY_APOLL_HPP_INCLUDED_
#define PYASYNCHIO_PY_APOLL_HPP_INCLUDED_

#pragma once

#include <python.h>
#include "socketmodule.h"
#include "fileobject.h"

namespace pyasynchio {

class PYASYNCHIO_LINK_DECL Py_apoll : public ::PyObject
{
    Py_apoll();
    ~Py_apoll();

    void accept(PySocketSockObject *so, PyObject *act);
    void connect(PySocketSockObject *so, PyObject *addro, PyObject *act);
    void send(PySocketSockObject *so, PyObject *datao, unsigned long flags
        , PyObject *act);
    void sendto(PySocketSockObject *so, PyObject *addro
        , PyObject *datao, PyObject *act);
    void recv(PySocketSockObject *so, unsigned long size, unsigned long flags
        , PyObject *act);
    void cancel(PyObject *o);
    void close(PyObject *o);

    PyObject* poll(unsigned long ms);

    
};


} // namespace pyasynchio

#endif // PYASYNCHIO_PY_APOLL_HPP_INCLUDED_