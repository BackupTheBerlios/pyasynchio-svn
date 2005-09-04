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

#include <pyasynchio/aio.hpp>
#include <mswsock.h>
#include <pyasynchio/socketmodule_stuff.hpp>


namespace pyasynchio {

::PyObject* AIO_ACCEPT::dump(BOOL success, DWORD bytes_transferred)
{
	PyObject *rp = aioresult::create();;
	PyObject_SetAttrString(rp, "lsock", lso_ref_);
	PyObject_SetAttrString(rp, "asock", aso_ref_);

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
            static_cast<int>(lfd_)                 // sockfd
            , local_addr                                    // addr
            , local_size                                    // addrlen
            , lproto_                              // proto
            );
        
        PyObject *remote_addro = makesockaddr(
            static_cast<int>(afd_)                 // sockfd
            , remote_addr                                   // addr
            , remote_size                                   // addrlen
            , aproto_                              // proto
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

::PyObject * AIO_CONNECT::dump(BOOL success, DWORD bytes_transferred)
{
	PyObject *rp = aioresult::create();;
	PyObject_SetAttrString(rp, "addr", addro_);
	return rp;
}

::PyObject * AIO_SEND::dump(BOOL success, DWORD bytes_transferred)
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

::PyObject * AIO_SENDTO::dump(BOOL success, DWORD bytes_transferred)
{
    PyObject * rp = AIO_SEND::dump(success, bytes_transferred);
    PyObject_SetAttrString(rp, "addr", addro_);
    return rp;
}

::PyObject * AIO_RECV::dump(BOOL success, DWORD bytes_transferred)
{
	::PyObject * rp = aioresult::create();;
	{
		::PyObject * bufsizeo = PyInt_FromLong(size_);
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

::PyObject * AIO_RECVFROM::dump(BOOL success, DWORD bytes_transferred)
{
    ::PyObject * rp = AIO_RECV::dump(success, bytes_transferred);
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

::PyObject * AIO_READ::dump(BOOL success, DWORD bytes_transferred)
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

::PyObject * AIO_WRITE::dump(BOOL success, DWORD bytes_transferred)
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
    //::PyObject * dp = Py_BuildValue("{sOsOsOsksO}"
    //    , "offset", offo
    //    , "data",  datao_
    //    , "act", acto_
    //    , "count", static_cast<unsigned long>(bytes_transferred)
    //    , "file", fo_);
    return rp;
}

} // namespace pyasynchio