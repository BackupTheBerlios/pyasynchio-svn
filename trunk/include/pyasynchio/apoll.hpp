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

#ifndef PYASYNCHIO_APOLL_HPP_INCLUDED_
#define PYASYNCHIO_APOLL_HPP_INCLUDED_

#pragma once

#include <pyasynchio/config.hpp>
#include <pyconfig.h>
#include <python.h>
#include "socketmodule.h"
#include "fileobject.h"
#include <windows.h>
#include <map>


namespace pyasynchio {

extern ::PyTypeObject apoll_Type;

class apoll : public ::PyObject, public platform::apoll_impl
{
public:
	apoll(::PyObject *args, ::PyObject *kwargs);
    ~apoll();

    bool accept(::PySocketSockObject *listen_socket
        , ::PySocketSockObject *accept_socket
        , ::PyObject *lsock_ref_obj
        , ::PyObject *asock_ref_obj
        , ::PyObject *act);
    bool connect(::PySocketSockObject *so, ::PyObject *so_ref
        , ::PyObject *addro, ::PyObject *acto);
    bool send(::PySocketSockObject *so, ::PyObject *so_ref
        , ::PyObject *datao, unsigned long flags
        , ::PyObject *acto);
    bool sendto(::PySocketSockObject *so, ::PyObject *so_ref
        , ::PyObject *addro
        , ::PyObject *datao
        , unsigned long flags
        , ::PyObject *acto);
    bool recv(::PySocketSockObject *so, ::PyObject *so_ref
        , unsigned long bufsize
        , unsigned long flags
        , ::PyObject *acto);
    bool recvfrom(::PySocketSockObject *so, ::PyObject *so_ref
        , unsigned long bufsize
        , unsigned long flags
        , ::PyObject *acto);
    bool cancel(::PyObject *o);
    void close(::PyObject *o);

    bool read(::PyFileObject *fo, unsigned long long offset, unsigned long size
        , ::PyObject *acto);
    bool write(::PyFileObject *fo, unsigned long long offset
        , ::PyObject *datao
        , ::PyObject *acto);
        

    ::PyObject* poll(long ms);

    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dealloc(apoll *self);
    static ::PyObject * accept_meth(apoll *self, ::PyObject *args);
    static ::PyObject * connect_meth(apoll *self, ::PyObject *args);
    static ::PyObject * send_meth(apoll *self, ::PyObject *args);
    static ::PyObject * sendto_meth(apoll *self, ::PyObject *args);
    static ::PyObject * recv_meth(apoll *self, ::PyObject *args);
    static ::PyObject * recvfrom_meth(apoll *self, ::PyObject *args);
    static ::PyObject * poll_meth(apoll *self, ::PyObject *args);
    static ::PyObject * cancel_meth(apoll *self, ::PyObject *args);

    static ::PyObject * read_meth(apoll *self, ::PyObject *args);
    static ::PyObject * write_meth(apoll *self, ::PyObject *args);

};

} // namespace pyasynchio

#endif // PYASYNCHIO_APOLL_HPP_INCLUDED_