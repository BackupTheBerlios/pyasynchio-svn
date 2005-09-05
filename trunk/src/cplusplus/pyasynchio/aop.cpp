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
#include <mswsock.h>
#include <pyasynchio/socketmodule_stuff.hpp>


namespace pyasynchio {

::PyObject* aop_accept::dump(BOOL success, DWORD bytes_transferred)
{
	PyObject *rp = aioresult::create();;
	PyObject_SetAttrString(rp, "lsock", lsock_refo_);
	PyObject_SetAttrString(rp, "asock", asock_refo_);

    if(success) {
        sockaddr *local_addr;
        int local_size = 0;
        sockaddr *remote_addr;
        int remote_size = 0;
        ::GetAcceptExSockaddrs(addr_buf_        // lpOutputBuffer
            , 0                                 // dwReceiveDataLength
            , addr_size                         // dwLocalAddressLength
            , addr_size                         // dwRemoteAddressLength
            , &local_addr                       // LocalSockaddr
            , &local_size                       // LocalSockadddrLength
            , &remote_addr                      // RemoteSockaddr
            , &remote_size                      // RemoteSockaddrLength
            );                  

        PyObject *local_addro = makesockaddr(
            static_cast<int>(lsocko_->sock_fd)                 // sockfd
            , local_addr                                    // addr
            , local_size                                    // addrlen
            , lsocko_->sock_proto                              // proto
            );
        
        PyObject *remote_addro = makesockaddr(
            static_cast<int>(asocko_->sock_fd)                 // sockfd
            , remote_addr                                   // addr
            , remote_size                                   // addrlen
            , asocko_->sock_proto                              // proto
            );

        PyObject_SetAttrString(rp, "laddr", local_addro);
        PyObject_SetAttrString(rp, "raddr", remote_addro);

        Py_DECREF(local_addro);
        Py_DECREF(remote_addro);
    }
	else {
		PyObject_SetAttrString(rp, "laddr", Py_None);
		PyObject_SetAttrString(rp, "raddr", Py_None);
	}

    return rp;
}

::PyObject * aop_connect::dump(BOOL success, DWORD bytes_transferred)
{
	PyObject *rp = aioresult::create();;
	PyObject_SetAttrString(rp, "addr", addro_);
	return rp;
}

::PyObject * aop_send::dump(BOOL success, DWORD bytes_transferred)
{
	::PyObject *rp = aioresult::create();;
	::PyObject_SetAttrString(rp, "sock", so_ref_);
	::PyObject_SetAttrString(rp, "data", datao_);
	{
		::PyObject *flagso = PyInt_FromLong(flags_);
		::PyObject_SetAttrString(rp, "flags", flagso);
		Py_DECREF(flagso);
	}
	{
		::PyObject *counto = PyInt_FromLong(bytes_transferred);
		::PyObject_SetAttrString(rp, "count", counto);
		Py_DECREF(counto);
	}

	return rp;
}

::PyObject * aop_sendto::dump(BOOL success, DWORD bytes_transferred)
{
    PyObject * rp = aop_send::dump(success, bytes_transferred);
    PyObject_SetAttrString(rp, "addr", addro_);
    return rp;
}

::PyObject * aop_recv::dump(BOOL success, DWORD bytes_transferred)
{
	::PyObject * rp = aioresult::create();;
	{
		::PyObject * bufsizeo = PyInt_FromLong(bufsize_);
		::PyObject_SetAttrString(rp, "bufsize", bufsizeo);
		Py_DECREF(bufsizeo);
	}
	::PyObject_SetAttrString(rp, "sock", so_ref_);
	{
		::PyObject * flagso = PyInt_FromLong(flags_);
		::PyObject_SetAttrString(rp, "flags", flagso);
		Py_DECREF(flagso);
	}

    if (success) {
        ::PyObject * bufo;
        bufo = ::PyString_FromStringAndSize(buf_, bytes_transferred);
        PyObject_SetAttrString(rp, "data", bufo);
        Py_DECREF(bufo);
    }
	else {
		PyObject_SetAttrString(rp, "data", Py_None);
	}

    return rp;
}

::PyObject * aop_recvfrom::dump(BOOL success, DWORD bytes_transferred)
{
    ::PyObject * rp = aop_recv::dump(success, bytes_transferred);
    if (success) {
        ::PyObject * addro = makesockaddr(
            static_cast<int>(fd_)
            , &from_
            , fromlen_
            , proto_);
        PyObject_SetAttrString(rp, "addr", addro);
        Py_DECREF(addro);
    }
	else {
		PyObject_SetAttrString(rp, "addr", Py_None);
	}

    return rp;
}

::PyObject * aop_read::dump(BOOL success, DWORD bytes_transferred)
{
	::PyObject * rp = aioresult::create();;
	{
	    ::PyObject * datao = ::PyString_FromStringAndSize(buf_, bytes_transferred);
		::PyObject_SetAttrString(rp, "data", datao);
	    Py_DECREF(datao);
	}
	{
		::PyObject * offo = ::PyLong_FromUnsignedLongLong(
			(static_cast<unsigned long long>(OffsetHigh) << (sizeof(DWORD)*8)) + Offset);
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

::PyObject * aop_write::dump(BOOL success, DWORD bytes_transferred)
{
	::PyObject * rp = aioresult::create();
	{
	    ::PyObject * offo = ::PyLong_FromUnsignedLongLong(
		    (static_cast<unsigned long long>(OffsetHigh) << (sizeof(DWORD)*8)) + Offset);
		::PyObject_SetAttrString(rp, "offset", offo);
	    Py_DECREF(offo);
	}
	::PyObject_SetAttrString(rp, "data", datao_);
	{
		::PyObject * counto = ::PyInt_FromLong(bytes_transferred);
		::PyObject_SetAttrString(rp, "count", counto);
		Py_DECREF(counto);
	}
	::PyObject_SetAttrString(rp, "file", reinterpret_cast<PyObject*>(fo_));
    return rp;
}

} // namespace pyasynchio