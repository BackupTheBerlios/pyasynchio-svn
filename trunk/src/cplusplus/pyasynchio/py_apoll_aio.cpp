#include <pyasynchio/py_apoll_aio.hpp>

namespace pyasynchio {

::PyObject* Py_apoll::AIO_ACCEPT::dump(BOOL success, DWORD bytes_transferred)
{

    PyObject *dp = Py_BuildValue("{sssisOsOsO}"
        , "type", "accept"
        , "success", static_cast<int>(success)
        , "listen_socket", lso_ref_
        , "accept_socket", aso_ref_
        , "act", acto_);

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

		PyDict_SetItemString(dp, "local_addr", local_addro);
		PyDict_SetItemString(dp, "remote_addr", remote_addro);

	    Py_XDECREF(local_addro);
		Py_XDECREF(remote_addro);
	}

    return dp;
}

::PyObject * Py_apoll::AIO_CONNECT::dump(BOOL success, DWORD bytes_transferred)
{
    return Py_BuildValue("{sssisOsOsO}"
        , "type", "connect"
        , "success", static_cast<int>(success)
        , "socket", so_ref_
        , "act", acto_
		, "addr", addro_);
}

::PyObject * Py_apoll::AIO_SEND::dump(BOOL success, DWORD bytes_transferred)
{
    return Py_BuildValue("{sssisOsOsOsksk}"
        , "type", "send"
        , "success", static_cast<int>(success)
        , "socket", so_ref_
        , "act", acto_
        , "data", datao_
        , "flags", flags_
        , "count", static_cast<unsigned long>(bytes_transferred));
}

::PyObject * Py_apoll::AIO_SENDTO::dump(BOOL success, DWORD bytes_transferred)
{
    PyObject * dp = Py_apoll::AIO_SEND::dump(success, bytes_transferred);
    PyDict_SetItemString(dp, "addr", addro_);
    PyObject * psendto = PyString_FromString("sendto");
    PyDict_SetItemString(dp, "type", psendto);
    Py_XDECREF(psendto);
    return dp;
}

::PyObject * Py_apoll::AIO_RECV::dump(BOOL success, DWORD bytes_transferred)
{
    PyObject * dp = Py_BuildValue("{sssislsOsOsk}"
        , "type", "recv"
        , "success", static_cast<int>(success)
        , "bufsize", size_
        , "socket", so_ref_
        , "act", acto_
		, "flags", flags_);

    if (success) {
	    ::PyObject * bufo;
        bufo = ::PyString_FromStringAndSize(buf_, bytes_transferred);
		PyDict_SetItemString(dp, "data", bufo);
	    Py_XDECREF(bufo);
    }

    return dp;
}

::PyObject * Py_apoll::AIO_RECVFROM::dump(BOOL success, DWORD bytes_transferred)
{
    ::PyObject * dp = Py_apoll::AIO_RECV::dump(success, bytes_transferred);
	{
		PyObject * precvfrom = PyString_FromString("recvfrom");
		PyDict_SetItemString(dp, "type", precvfrom);
		Py_XDECREF(precvfrom);
	}
	if (success) {
		::PyObject * addro = makesockaddr(
			static_cast<int>(fd_)
			, &from_
			, fromlen_
			, proto_);
		PyDict_SetItemString(dp, "addr", addro);
		Py_XDECREF(addro);
	}

    return dp;
}

::PyObject * Py_apoll::AIO_READ::dump(BOOL success, DWORD bytes_transferred)
{
    ::PyObject * datao = ::PyString_FromStringAndSize(buf_, bytes_transferred);
    ::PyObject * offo = ::PyLong_FromUnsignedLongLong(
        (static_cast<unsigned long long>(OffsetHigh) << (sizeof(DWORD)*8)) + Offset);
    ::PyObject * dp =  Py_BuildValue("{sssisOslsOsOsO}"
        , "type", "read"
        , "success", static_cast<int>(success)
        , "offset", offo
        , "bufsize", size_
        , "act", acto_
        , "data", datao
        , "file", fo_);
    Py_XDECREF(datao);
    Py_XDECREF(offo);
    return dp;
}

::PyObject * Py_apoll::AIO_WRITE::dump(BOOL success, DWORD bytes_transferred)
{
    ::PyObject * offo = ::PyLong_FromUnsignedLongLong(
        (static_cast<unsigned long long>(OffsetHigh) << (sizeof(DWORD)*8)) + Offset);
    ::PyObject * dp = Py_BuildValue("{sssisOsOsOsksO}"
        , "type", "write"
        , "success", static_cast<int>(success)
        , "offset", offo
        , "data",  datao_
        , "act", acto_
        , "count", static_cast<unsigned long>(bytes_transferred)
        , "file", fo_);
    Py_XDECREF(offo);
    return dp;
}

} // namespace pyasynchio