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

#include <pyasynchio/aop.hpp>
#include <pyasynchio/socketmodule_stuff.hpp>


namespace pyasynchio {

::PyObject* aop_accept::dump(bool success, size_t bytes_count)
{
    PyObject *rp = aioresult::create();;
    PyObject_SetAttrString(rp, "lsock", lsock_refo_);
    PyObject_SetAttrString(rp, "asock", asock_refo_);
    return rp;
}

::PyObject * aop_connect::dump(bool success, size_t bytes_transferred)
{
    PyObject *rp = aioresult::create();;
    PyObject_SetAttrString(rp, "addr", addro_);
    return rp;
}

::PyObject * aop_send::dump(bool success, size_t bytes_transferred)
{
    ::PyObject *rp = aioresult::create();
    ::PyObject_SetAttrString(rp, "sock", sock_refo_);
    ::PyObject_SetAttrString(rp, "data", datao_);
    {
        ::PyObject *flagso = PyInt_FromLong(flags_);
        ::PyObject_SetAttrString(rp, "flags", flagso);
        Py_DECREF(flagso);
    }
    {
        ::PyObject *counto = PyInt_FromLong(static_cast<long>(bytes_transferred));
        ::PyObject_SetAttrString(rp, "count", counto);
        Py_DECREF(counto);
    }

    return rp;
}

::PyObject * aop_sendto::dump(bool success, size_t bytes_transferred)
{
    PyObject * rp = aop_send::dump(success, bytes_transferred);
    PyObject_SetAttrString(rp, "addr", addro_);
    return rp;
}

::PyObject * aop_recv::dump(bool success, size_t bytes_transferred)
{
    ::PyObject * rp = aioresult::create();;
    {
        ::PyObject * bufsizeo = PyInt_FromLong(bufsize_);
        ::PyObject_SetAttrString(rp, "bufsize", bufsizeo);
        Py_DECREF(bufsizeo);
    }
    ::PyObject_SetAttrString(rp, "sock", sock_refo_);
    {
        ::PyObject * flagso = PyInt_FromLong(flags_);
        ::PyObject_SetAttrString(rp, "flags", flagso);
        Py_DECREF(flagso);
    }

    if (success) {
        ::PyObject * bufo;
        bufo = ::PyString_FromStringAndSize(buf_, static_cast<long>(bytes_transferred));
        PyObject_SetAttrString(rp, "data", bufo);
        Py_DECREF(bufo);
    }
    else {
        PyObject_SetAttrString(rp, "data", Py_None);
    }

    return rp;
}

::PyObject * aop_recvfrom::dump(bool success, size_t bytes_transferred)
{
    ::PyObject * rp = aop_recv::dump(success, bytes_transferred);
    if (success) {
        ::PyObject * addro = makesockaddr(
            static_cast<int>(socko_->sock_fd)
            , &from_
            , fromlen_
            , socko_->sock_proto);
        PyObject_SetAttrString(rp, "addr", addro);
        Py_DECREF(addro);
    }
    else {
        PyObject_SetAttrString(rp, "addr", Py_None);
    }

    return rp;
}

::PyObject * aop_read::dump(bool success, size_t bytes_transferred)
{
    ::PyObject * rp = aioresult::create();;
    {
        ::PyObject * datao = ::PyString_FromStringAndSize(buf_
            , static_cast<long>(bytes_transferred));
        ::PyObject_SetAttrString(rp, "data", datao);
        Py_DECREF(datao);
    }
    {
        ::PyObject * offo = ::PyLong_FromUnsignedLongLong(off_);
        ::PyObject_SetAttrString(rp, "offset", offo);
        Py_DECREF(offo);
    }
    {
        ::PyObject * bufsizeo = ::PyInt_FromLong(size_);
        PyObject_SetAttrString(rp, "bufsize", bufsizeo);
        Py_DECREF(bufsizeo);
    }
    ::PyObject_SetAttrString(rp, "file", reinterpret_cast<PyObject*>(fo_));
    return rp;
}

::PyObject * aop_write::dump(bool success, size_t bytes_transferred)
{
    ::PyObject * rp = aioresult::create();
    {
        ::PyObject * offo = ::PyLong_FromUnsignedLongLong(off_);
        ::PyObject_SetAttrString(rp, "offset", offo);
        Py_DECREF(offo);
    }
    ::PyObject_SetAttrString(rp, "data", datao_);
    {
        ::PyObject * counto = ::PyInt_FromLong(static_cast<long>(bytes_transferred));
        ::PyObject_SetAttrString(rp, "count", counto);
        Py_DECREF(counto);
    }
    ::PyObject_SetAttrString(rp, "file", reinterpret_cast<PyObject*>(fo_));
    return rp;
}

} // namespace pyasynchio