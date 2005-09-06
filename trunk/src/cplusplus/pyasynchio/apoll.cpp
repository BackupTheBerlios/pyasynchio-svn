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


#include <pyasynchio/apoll.hpp>
#include <pyasynchio/utils.hpp>
#include <pyasynchio/aop.hpp>
#include <pyasynchio/socketmodule_stuff.hpp>
#include <new>
#include <internal.h>
#include <winternl.h>
#include <mswsock.h>
#include <string>

namespace pyasynchio {

apoll::apoll(::PyObject *args, ::PyObject *kwargs)
: apoll_impl(args, kwargs)
{
}

apoll::~apoll()
{
}

bool apoll::accept(::PySocketSockObject *lsocko
                      , ::PySocketSockObject *asocko
                      , ::PyObject *lsock_ref
                      , ::PyObject *asock_ref
                      , ::PyObject *acto)
{
    aop_accept *aaop = new aop_accept(acto				// acto
        , lsocko                                        // lso
        , asocko                                        // aso
        , lsock_ref                                     // lso_ref
        , asock_ref                                     // aso_ref
		);
    bool ar = accept_impl(aaop);
    if (!ar) {
        delete aaop;
    }
    return ar;
}

bool apoll::recv(::PySocketSockObject *so
                    , ::PyObject *so_ref
                    , unsigned long bufsize
                    , unsigned long flags
                    , ::PyObject *acto)
{
    aop_recv * arop = new aop_recv(acto, so, so_ref, bufsize, flags);
	bool rr = recv_impl(arop);
	if (!rr) {
		delete arop;
	}
	return rr;
}

bool apoll::recvfrom(::PySocketSockObject * so
                        , ::PyObject *so_ref
                        , unsigned long bufsize
                        , unsigned long flags
                        , ::PyObject * acto)
{
    aop_recvfrom *arfop = new aop_recvfrom(acto, so, so_ref, bufsize, flags);
	bool rfr = recvfrom_impl(arfop);
	if (!rfr) {
		delete arfop;
	}
	return rfr;
}

bool apoll::sendto(::PySocketSockObject *so, ::PyObject *so_ref
                      , ::PyObject *addro
                      , ::PyObject *datao
                      , unsigned long flags
                      , ::PyObject *acto)
{
    aop_sendto *asynch_sendto_op = new aop_sendto(acto, so, so_ref, addro
												, flags, datao);
	bool sendto_result = sendto_impl(asynch_sendto_op);
	if (!sendto_result) {
		delete asynch_sendto_op;
	}
	return sendto_result;
}

bool apoll::send(::PySocketSockObject *so, ::PyObject *so_ref
                    , ::PyObject *datao, unsigned long flags
                    , ::PyObject *acto)
{

    aop_send * asynch_send_op = new aop_send(acto, so, so_ref, flags, datao);
	bool send_result = send_impl(asynch_send_op);
	if (!send_result) {
		delete asynch_send_op;
	}
	return send_result;
}

bool apoll::connect(::PySocketSockObject *so, ::PyObject *so_ref
                       , ::PyObject *addro, ::PyObject *acto)
{
    aop_connect * acop = new aop_connect(acto, so, so_ref, addro);
    bool cr = connect_impl(acop);
    if (!cr) {
        delete acop;
    }
    return cr;
}

bool apoll::cancel(PyObject *o)
{
    HANDLE h;
    if(PyObject_IsInstance(o, reinterpret_cast<PyObject*>(socketmodule_api.Sock_Type))) {
        PySocketSockObject * so = reinterpret_cast<PySocketSockObject*>(o);
        h = reinterpret_cast<HANDLE>(so->sock_fd);
    }
    else {
        if (PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyFile_Type))) {
            PyFileObject *fo = reinterpret_cast<PyFileObject*>(o);
            h = get_file_handle(fo);
        }
        else {
            PyErr_SetString(PyExc_TypeError, "unrecognized I/O object type");
            return false;
        }
    }
    if(! ::CancelIo(h)) {
        PyErr_SetString(PyExc_RuntimeError, "::CancelIo failed");
        return false;
    }
    return true;
}

bool apoll::read(PyFileObject *fo, unsigned long long offset, unsigned long size
                    , ::PyObject *acto)
{
    aop_read * arop = new aop_read(acto, fo, offset, size);
	bool rr = read_impl(arop);
	if(!rr) {
		delete arop;
	}
	return rr;
}

bool apoll::write(PyFileObject *fo, unsigned long long offset
                     , ::PyObject *datao, ::PyObject *acto)
{
    aop_write * awop = new aop_write(acto, fo, offset, datao);
	bool wr = write_impl(awop);
	if(!wr) {
		delete awop;
	}
	return wr;
}

::PyObject * apoll::poll(long ms)
{
    DWORD bytes_transferred = 0;
    ULONG_PTR completion_key = 0;
    aop_root *ovr = 0;
    PyObject * result = PyList_New(0);

    for(;;) {
        BOOL success = FALSE;
        OVERLAPPED *ovl = NULL;
        Py_BEGIN_ALLOW_THREADS;
        success = ::GetQueuedCompletionStatus(iocp_handle_  // CompletionPort
            , &bytes_transferred                                // lpNumberOfBytesTransferred
            , &completion_key                                   // lpCompletionKey
            , &ovl                                              // lpOverlapped
            , ms                                                // dwMilliseconds
            );
        ms = 0; // only first request should wait, so timeout is zero for subsequent ops
        Py_END_ALLOW_THREADS;
        ovr = static_cast<aop_root*>(ovl);

        if ( (0 == ovl) && (FALSE == success) ) {
            if (WAIT_TIMEOUT != ::GetLastError()) {
                PyErr_SetString(PyExc_RuntimeError, "::GetQueuedCompletionStatus failed");
                return NULL;
            }
            else {
                return result;
            }
        }

        if ( 0 == ovl) {
            PyErr_SetString(PyExc_RuntimeError
                , "something impossible happened in ::GetQueuedCompletionStatus");
            return NULL;
        }

        PyObject *op_result = ovr->to_python(success, bytes_transferred);
        PyList_Append(result, op_result);
        Py_XDECREF(op_result);
        delete ovr;
    }

    return result;
}

static PyMethodDef apoll_methods[] = {
    { "accept"
        , reinterpret_cast<PyCFunction>(&apoll::accept_meth)
        , METH_VARARGS
        , "start asynchronous accept operation"
    }
    , { "connect"
        , reinterpret_cast<PyCFunction>(&apoll::connect_meth)
        , METH_VARARGS
        , "start asynchronou connect operation"
    }
    , { "send"
        , reinterpret_cast<PyCFunction>(&apoll::send_meth)
        , METH_VARARGS
        , "start asynchronous send operation"
    }
    , { "sendto"
        , reinterpret_cast<PyCFunction>(&apoll::sendto_meth)
        , METH_VARARGS
        , "start asynchronous sendto operation"
    }
    , { "recv"
        , reinterpret_cast<PyCFunction>(&apoll::recv_meth)
        , METH_VARARGS
        , "start asynchronous recv operation"
    }
    , { "recvfrom"
        , reinterpret_cast<PyCFunction>(&apoll::recvfrom_meth)
        , METH_VARARGS
        , "start asynchronous recvfrom operation"
    }
    , { "poll"
        , reinterpret_cast<PyCFunction>(&apoll::poll_meth)
        , METH_VARARGS
        , "poll asynchronous operation results"
    }
    , { "cancel"
        , reinterpret_cast<PyCFunction>(&apoll::cancel_meth)
        , METH_VARARGS
        , "cancel asynchronous operations for given object"
    }
    , { "read"
        , reinterpret_cast<PyCFunction>(&apoll::read_meth)
        , METH_VARARGS
        , "start asynchronous read operation on file"
    }
    , { "write"
        , reinterpret_cast<PyCFunction>(&apoll::write_meth)
        , METH_VARARGS
        , "start asynchronous write operation on file" 
    }
    , {NULL} /* Sentinel */
};

PyTypeObject apoll_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_pyasynchio.apoll",             /*tp_name*/
    sizeof(apoll), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)apoll::dealloc,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "apoll objects",           /* tp_doc */

    0,                                  // tp_traverse 
    0,                                  // tp_clear
    0,                                  // tp_richcompare 
    0,                                  // tp_weaklistoffset 

    0,                                  // tp_iter
    0,                                  // tp_iternext 

    apoll_methods,                      // tp_methods 
    0,                                  // tp_members
    0,                                  // tp_getset
    0,                                  // tp_base 
    
    0,                                  // tp_dict
    0,                                  // tp_descr_get
    0,                                  // tp_descr_set
    0,                                  // tp_dictoffset 
    apoll::init,               // tp_init
    0,                                  // tp_alloc 
    PyType_GenericNew,                 // tp_new
    0,                                  // tp_free
    0,                                  // tp_is_gc
    0,                                  // tp_bases
    0,                                  // tp_mro
    0,                                  // tp_cache
    0,                                  // tp_subclasses
    0,                                  // tp_weaklist
    0,                                  // tp_del
};


int apoll::init(PyObject *self, PyObject *args, PyObject *kwds)
{
    try {
        new (self) pyasynchio::apoll(args, kwds);
    }
    catch(std::exception &)
    {
        return FALSE;
    }
    return TRUE;
}

void apoll::dealloc(pyasynchio::apoll *self)
{
    self->~apoll();
    self->ob_type->tp_free(self);
}

PyObject* apoll::accept_meth(apoll *self, ::PyObject *args)
{
    PyObject *listen_sock_raw, *accept_sock_raw
        , *lsock_ref_obj, *asock_ref_obj
        , *act;
    if (!PyArg_ParseTuple(args, "OOOOO:accept", &listen_sock_raw, &accept_sock_raw
        , &lsock_ref_obj, &asock_ref_obj
        , &act)) {
        return NULL;
    }

    PySocketSockObject *listen_sock = py_convert<PySocketSockObject>(listen_sock_raw
        , socketmodule_api.Sock_Type);
    if (NULL == listen_sock) {
        return NULL;
    }

    PySocketSockObject *accept_sock = py_convert<PySocketSockObject>(accept_sock_raw
        , socketmodule_api.Sock_Type);
    if (NULL == accept_sock) {
        return NULL;
    }

    if(!self->accept(listen_sock, accept_sock, lsock_ref_obj, asock_ref_obj, act)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::connect_meth(apoll *self, ::PyObject *args)
{
    PyObject *so_raw, *so_ref, *addro, *acto;
    if (!PyArg_ParseTuple(args, "OOOO:connect", &so_raw, &so_ref, &addro, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->connect(so, so_ref, addro, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::sendto_meth(apoll * self, ::PyObject * args)
{
    ::PyObject *so_raw, *so_ref, *addro, *datao, *acto;
    unsigned long flags;
    if (!PyArg_ParseTuple(args, "OOOOkO:sendto", &so_raw, &so_ref, &addro, &datao, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->sendto(so, so_ref, addro, datao, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::send_meth(apoll * self, ::PyObject * args)
{
    ::PyObject *so_raw, *so_ref, *datao, *acto;
    unsigned long flags;
    if (!PyArg_ParseTuple(args, "OOOkO:send", &so_raw, &so_ref
        , &datao, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->send(so, so_ref, datao, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::recv_meth(apoll *self, ::PyObject *args)
{
    ::PyObject *so_raw, *so_ref, *acto;
    unsigned long flags, size;
    if (!PyArg_ParseTuple(args, "OOkkO:recv", &so_raw, &so_ref
        , &size, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->recv(so, so_ref, size, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::recvfrom_meth(apoll *self, ::PyObject *args)
{
    ::PyObject *so_raw, *so_ref, *acto;
    unsigned long flags, size;
    if (!PyArg_ParseTuple(args, "OOkkO:recvfrom", &so_raw, &so_ref
        , &size, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->recvfrom(so, so_ref, size, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::poll_meth(apoll *self, ::PyObject *args)
{
    long ms;
    if (!PyArg_ParseTuple(args, "l:poll", &ms)) {
        return NULL;
    }

    return self->poll(ms);
}

::PyObject * apoll::cancel_meth(apoll * self, ::PyObject * args)
{
    ::PyObject * o;
    if (!PyArg_ParseTuple(args, "O:cancel", &o)) {
        return NULL;
    }

    if (!self->cancel(o)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::write_meth(apoll * self, ::PyObject * args)
{
    ::PyObject * fo_raw, *acto, *datao;
    unsigned long long offset = 0;
    if(!PyArg_ParseTuple(args, "OKOO:write", &fo_raw, &offset, &datao, &acto)) {
        return NULL;
    }

    PyFileObject * fo = py_convert<PyFileObject>(fo_raw, &PyFile_Type);
    if(NULL == fo) {
        return NULL;
    }

    if(!self->write(fo, offset, datao, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * apoll::read_meth(apoll * self, ::PyObject * args)
{
    ::PyObject * fo_raw, *acto;
    unsigned long long offset = 0;
    unsigned long size = 0;
    if (!PyArg_ParseTuple(args, "OKkO:read", &fo_raw, &offset, &size, &acto)) {
        return NULL;
    }

    PyFileObject * fo = py_convert<PyFileObject>(fo_raw, &PyFile_Type);
    if (NULL == fo) {
        return NULL;
    }

    if (!self->read(fo, offset, size, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

} // namespace pyasynchio