#include <pyasynchio/py_apoll.hpp>
#include <pyasynchio/socketmodule_stuff.hpp>
#include <mswsock.h>
#include <new>

namespace pyasynchio {

Py_apoll::Py_apoll()
{
    iocp_handle_ = ::CreateIoCompletionPort(
        INVALID_HANDLE_VALUE // File handle 
        , NULL // existing completion port
        , NULL // completion key
        , 0 // number of concurrent threads
    );
    if (NULL == iocp_handle_) {
        PyErr_SetString(PyExc_RuntimeError, "::CreateIoCompletionPort failed");
    }
}

Py_apoll::~Py_apoll()
{
    ::CloseHandle(iocp_handle_);
}

bool Py_apoll::iocp_preamble(HANDLE h)
{
    HANDLE iocp_ret = ::CreateIoCompletionPort( h
        , iocp_handle_
        , NULL
        , 0);
    if (iocp_handle_ != iocp_ret) {
        DWORD error = GetLastError();
        if (error != ERROR_INVALID_PARAMETER) {
            ::PyErr_Format(PyExc_RuntimeError
                , "Failed to associate I/O completion port with socket handle %d"
                , GetLastError());
            return false;
        }
    }
    return true;
}

bool Py_apoll::accept(::PySocketSockObject *listen_sock
                      , ::PySocketSockObject *accept_sock
                      , ::PyObject *acto)
{
    if (!iocp_preamble(reinterpret_cast<HANDLE>(listen_sock->sock_fd))) {
        return false;
    }
    OVL_ACCEPT *ova = new OVL_ACCEPT(acto               // acto
        , listen_sock                                   // lso
        , accept_sock       // aso
        );
    DWORD num_bytes_rcvd = 0;
    BOOL r = ::AcceptEx(listen_sock->sock_fd    // sListenSocket 
        , accept_sock->sock_fd                  // sAcceptSocket 
        , &ova->addr_buf_[0]                        // lpOutputBuffer
        , 0                                     // dwReceiveDataLength
        , addr_size                             // dwLocalAddressLength
        , addr_size                             // dwRemoteAddressLength
        , &num_bytes_rcvd                       // lpdwBytesReceived
        , ova   // lpOverlapped                     
        );                                  
    return check_wsa_op(r, TRUE, "::AcceptEx failed");
}

bool Py_apoll::recv(::PySocketSockObject *so, unsigned long bufsize
                    , unsigned long flags
                    , ::PyObject *acto)
{
    if (!iocp_preamble(reinterpret_cast<HANDLE>(so->sock_fd))) {
        return false;
    }
    OVL_RECV * ovr = new OVL_RECV(acto, so, bufsize, flags);
    WSABUF wb;
    wb.buf = ovr->buf();
    wb.len = bufsize;
    DWORD bytes_rcvd = 0;
    int r = ::WSARecv(so->sock_fd               // SOCKET s
        , &wb                               // LPWSABUF lpBuffers
        , 1                                 // DWORD dwBufferCount
        , &bytes_rcvd                       // LPDWORD lpNumberOfBytesRecvd
        , &flags                            // LPDWORD lpFlags
        , ovr                               // LPWSAOVERLAPPED lpOverlapped
        , 0                                 // LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );
    return check_wsa_op(r, 0, "::WSARecv failed");
}

bool Py_apoll::recvfrom(::PySocketSockObject * so, unsigned long bufsize
                        , unsigned long flags
                        , ::PyObject * acto)
{
    if (!iocp_preamble(reinterpret_cast<HANDLE>(so->sock_fd))) {
        return false;
    }
    OVL_RECVFROM *ovf = new OVL_RECVFROM(acto, so, bufsize, flags);
    WSABUF wb;
    wb.buf = ovf->buf();
    wb.len = bufsize;
    DWORD bytes_rcvd = 0;
    int r = ::WSARecvFrom(so->sock_fd               // SOCKET s
        , &wb                                       // LPWSABUF lpBuffers
        , 1                                         // DWORD dwBufferCount
        , &bytes_rcvd                               // LPDWORD lpNumberOfBytesRecvd
        , &flags                                    // LPDWORD lpFlags
        , ovf->from()                               // sockaddr *lpFrom
        , ovf->fromlen()                            // LPINT lpFromlen
        , ovf                                       // LPWSAOVERLAPPED lpOverlapped
        , 0                                         // LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );
    return check_wsa_op(r, 0, "::WSARecvFrom failed");
}

bool Py_apoll::sendto(::PySocketSockObject *so, ::PyObject *addro
                      , ::PyObject *datao
                      , unsigned long flags
                      , ::PyObject *acto)
{
    if (!iocp_preamble(reinterpret_cast<HANDLE>(so->sock_fd))) {
        return false;
    }

    sockaddr addr;
    int addr_len = 0;
    if (!getsockaddrarg(so, addro, &addr, &addr_len)) {
        return false;
    }
    
    int len;
    char *s;
    if(-1 == PyString_AsStringAndSize(datao, &s, &len)) {
        return NULL;
    }
    WSABUF wb;
    wb.buf = s;
    wb.len = len;

    OVL_SENDTO *ovt = new OVL_SENDTO(acto, so, addro, flags, datao);

    DWORD bytes_sent = 0;
    int r = ::WSASendTo(so->sock_fd             // SOCKET s
        , &wb                                   // LPWSABUF lpBuffers
        , 1                                     // DWORD dwBufferCount
        , &bytes_sent                           // LPDWORD lpNumberOfBytesSent
        , flags                                 // DWORD dwFlags
        , &addr                                 // const sockaddr * lpTo
        , addr_len                              // int iToken
        , ovt                                   // LPWSAOVERLAPPED lpOverlapped
        , 0                                     // LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );
    return check_wsa_op(r, 0, "::WSASendTo failed");
}

bool Py_apoll::send(::PySocketSockObject *so, ::PyObject *datao, unsigned long flags
                    , ::PyObject *acto)
{
    if (!iocp_preamble(reinterpret_cast<HANDLE>(so->sock_fd))) {
        return false;
    }

    char *s;
    int len;
    if(-1 == PyString_AsStringAndSize(datao, &s, &len)) {
        return NULL;
    }
    WSABUF wb;
    wb.buf = s;
    wb.len = len;
    OVL_SEND * ovs = new OVL_SEND(acto, so, flags, datao);
    DWORD bytes_sent = 0;
    int r = ::WSASend(so->sock_fd               // SOCKET s
        , &wb                               // LPWSABUF lpBuffers
        , 1                                 // DWORD dwBufferCount
        , &bytes_sent                       // LPDWORD lpNumberOfBytesSent
        , flags                             // DWORD dwFlags
        , ovs                               // LPWSAOVERLAPPED lpOverlapped
        , NULL                              // LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
        );
    return check_wsa_op(r, 0, "::WSASend failed");
}

bool Py_apoll::connect(::PySocketSockObject *so, ::PyObject *addro, ::PyObject *acto)
{
    if (!iocp_preamble(reinterpret_cast<HANDLE>(so->sock_fd))) {
        return false;
    }
    sockaddr addr;
    int addr_len = 0;
    if (!getsockaddrarg(so, addro, &addr, &addr_len)) {
        return false;
    }
    OVL_CONNECT * ovc = new OVL_CONNECT(acto, so);
    GUID GuidConnectEx = WSAID_CONNECTEX;
    ::LPFN_CONNECTEX ConnectEx;
    DWORD dwBytes;
    if (WSAIoctl(so->sock_fd, 
        SIO_GET_EXTENSION_FUNCTION_POINTER, 
        &GuidConnectEx, 
        sizeof(GuidConnectEx),
        &ConnectEx, 
        sizeof(ConnectEx), 
        &dwBytes, 
        NULL, 
        NULL) == SOCKET_ERROR) 
    {
        PyErr_SetString(PyExc_RuntimeError, "ConnectEx not found with WSAIoctl");
        return false;
    }
    DWORD bytes_sent = 0;
    BOOL r = ConnectEx(so->sock_fd          // s
        , &addr                                 // name
        , addr_len                              // namelen
        , 0                                     // lpSendBuffer
        , 0                                     // dwSendDataLength
        , &bytes_sent                           // lpdwBytesSent
        , ovc                                   // lpOverlapped
        );
    return check_wsa_op(r, TRUE, "::ConnectEx failed");
}

bool Py_apoll::cancel(PyObject *o)
{
	HANDLE h;
	if(PyObject_IsInstance(o, reinterpret_cast<PyObject*>(PySocketModule.Sock_Type))) {
		PySocketSockObject * so = reinterpret_cast<PySocketSockObject*>(o);
		h = reinterpret_cast<HANDLE>(so->sock_fd);
	}
	else {
		return false;
	}
	if(! ::CancelIo(h)) {
		PyErr_SetString(PyExc_RuntimeError, "::CancelIo failed");
		return false;
	}
	return true;
}

::PyObject * Py_apoll::poll(unsigned long ms)
{
    DWORD bytes_transferred = 0;
    ULONG_PTR completion_key = 0;
    OVL_ROOT *ovr = 0;
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
        ovr = static_cast<OVL_ROOT*>(ovl);

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

        PyList_Append(result, ovr->dump(success, bytes_transferred));
        delete ovr;
    }

    return result;
}

static PyTypeObject apoll_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pyasynchio.apoll",             /*tp_name*/
    sizeof(Py_apoll), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Apoll objects",           /* tp_doc */
};

::PyObject* Py_apoll::OVL_ACCEPT::dump(BOOL success, DWORD bytes_transferred)
{
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
        static_cast<int>(lso_->sock_fd)                 // sockfd
        , local_addr                                    // addr
        , local_size                                    // addrlen
        , lso_->sock_proto                              // proto
        );
    
    PyObject *remote_addro = makesockaddr(
        static_cast<int>(aso_->sock_fd)                 // sockfd
        , remote_addr                                   // addr
        , remote_size                                   // addrlen
        , aso_->sock_proto                              // proto
        );

    return Py_BuildValue("{sssisOsOsOsOsO}"
        , "type", "accept"
        , "success", static_cast<int>(success)
        , "local_addr", local_addro
        , "remote_addr", remote_addro
        , "listen_socket", lso_
        , "accept_socket", aso_
        , "act", acto_);
}

::PyObject * Py_apoll::OVL_CONNECT::dump(BOOL success, DWORD bytes_transferred)
{
    return Py_BuildValue("{sssisOsO}"
        , "type", "connect"
        , "success", static_cast<int>(success)
        , "socket", so_
        , "act", acto_);
}

::PyObject * Py_apoll::OVL_SEND::dump(BOOL success, DWORD bytes_transferred)
{
    return Py_BuildValue("{sssisOsOsOsk}"
        , "type", "send"
        , "success", static_cast<int>(success)
        , "socket", so_
        , "act", acto_
        , "data", datao_
        , "flags", flags_);
}

::PyObject * Py_apoll::OVL_SENDTO::dump(BOOL success, DWORD bytes_transferred)
{
    PyObject * dp = Py_apoll::OVL_SEND::dump(success, bytes_transferred);
    PyDict_SetItemString(dp, "addr", addro_);
    PyDict_SetItemString(dp, "type", PyString_FromString("sendto"));
    return dp;
}

::PyObject * Py_apoll::OVL_RECV::dump(BOOL success, DWORD bytes_transferred)
{
    ::PyObject * bufo;
    if (success) {
        bufo = ::PyString_FromStringAndSize(buf_, bytes_transferred);
    }
    else {
        bufo = ::PyString_FromString("");
    }
    
    return Py_BuildValue("{sssislsOsOsO}"
        , "type", "recv"
        , "success", static_cast<int>(success)
        , "bufsize", size_
        , "socket", so_
        , "act", acto_
        , "data", bufo);
}

::PyObject * Py_apoll::OVL_RECVFROM::dump(BOOL success, DWORD bytes_transferred)
{
    ::PyObject * dp = Py_apoll::OVL_RECV::dump(success, bytes_transferred);
    ::PyObject * addro = makesockaddr(
        static_cast<int>(so_->sock_fd)
        , &from_
		, fromlen_
		, so_->sock_proto);
    PyDict_SetItemString(dp, "type", PyString_FromString("recvfrom"));
	PyDict_SetItemString(dp, "addr", addro);
	return dp;
}

int Py_apoll::init_func(PyObject *self, PyObject *args, PyObject *kwds)
{
    char* keywords[] = { NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwds, ":apoll", keywords)) {
        return NULL;
    }
    new (self) pyasynchio::Py_apoll;
    return TRUE;
}

PyObject* Py_apoll::accept_meth(Py_apoll *self, ::PyObject *args)
{
    PyObject *listen_sock_raw, *accept_sock_raw, *act;
    if (!PyArg_ParseTuple(args, "OOO:accept", &listen_sock_raw, &accept_sock_raw, &act)) {
        return NULL;
    }

    PySocketSockObject *listen_sock = py_convert<PySocketSockObject>(listen_sock_raw
        , PySocketModule.Sock_Type);
    if (NULL == listen_sock) {
        return NULL;
    }

    PySocketSockObject *accept_sock = py_convert<PySocketSockObject>(accept_sock_raw
        , PySocketModule.Sock_Type);
    if (NULL == accept_sock) {
        return NULL;
    }

    if(!self->accept(listen_sock, accept_sock, act)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::connect_meth(Py_apoll *self, ::PyObject *args)
{
    PyObject *so_raw, *addro, *acto;
    if (!PyArg_ParseTuple(args, "OOO:connect", &so_raw, &addro, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->connect(so, addro, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::sendto_meth(Py_apoll * self, ::PyObject * args)
{
    ::PyObject *so_raw, *addro, *datao, *acto;
    unsigned long flags;
    if (!PyArg_ParseTuple(args, "OOOkO:sendto", &so_raw, &addro, &datao, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->sendto(so, addro, datao, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::send_meth(Py_apoll * self, ::PyObject * args)
{
    ::PyObject *so_raw, *datao, *acto;
    unsigned long flags;
    if (!PyArg_ParseTuple(args, "OOkO:send", &so_raw, &datao, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->send(so, datao, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::recv_meth(Py_apoll *self, ::PyObject *args)
{
    ::PyObject *so_raw, *acto;
    unsigned long flags, size;
    if (!PyArg_ParseTuple(args, "OkkO:recv", &so_raw, &size, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->recv(so, size, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::recvfrom_meth(Py_apoll *self, ::PyObject *args)
{
    ::PyObject *so_raw, *acto;
    unsigned long flags, size;
    if (!PyArg_ParseTuple(args, "OkkO:recvfrom", &so_raw, &size, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->recvfrom(so, size, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::poll_meth(Py_apoll *self, ::PyObject *args)
{
    unsigned long ms;
	if (!PyArg_ParseTuple(args, "k:poll", &ms)) {
        return NULL;
    }

    return self->poll(ms);
}

::PyObject * Py_apoll::cancel_meth(Py_apoll * self, ::PyObject * args)
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

static PyMethodDef pyasynchio_methods[] = {
    {NULL}  /* Sentinel */
};

static PyMethodDef apoll_methods[] = {
    { "accept"
        , reinterpret_cast<PyCFunction>(&Py_apoll::accept_meth)
        , METH_VARARGS
        , "start asynchronous accept operation"
    }
    , { "connect"
        , reinterpret_cast<PyCFunction>(&Py_apoll::connect_meth)
        , METH_VARARGS
        , "start asynchronou connect operation"
    }
    , { "send"
        , reinterpret_cast<PyCFunction>(&Py_apoll::send_meth)
        , METH_VARARGS
        , "start asynchronous send operation"
    }
    , { "sendto"
        , reinterpret_cast<PyCFunction>(&Py_apoll::sendto_meth)
        , METH_VARARGS
        , "start asynchronous sendto operation"
    }
    , { "recv"
        , reinterpret_cast<PyCFunction>(&Py_apoll::recv_meth)
        , METH_VARARGS
        , "start asynchronous recv operation"
    }
    , { "recvfrom"
        , reinterpret_cast<PyCFunction>(&Py_apoll::recvfrom_meth)
        , METH_VARARGS
        , "start asynchronous recvfrom operation"
    }
    , { "poll"
        , reinterpret_cast<PyCFunction>(&Py_apoll::poll_meth)
        , METH_VARARGS
        , "poll asynchronous operation results"
    }
	, { "cancel"
		, reinterpret_cast<PyCFunction>(&Py_apoll::cancel_meth)
		, METH_VARARGS
		, "cancel asynchronous operations for given object"
	}
    , {NULL} /* Sentinel */
};


void initpyasynchio()
{
    ::PySocketModule_ImportModuleAndAPI();

    apoll_Type.tp_methods = apoll_methods;
    apoll_Type.tp_new = PyType_GenericNew;
    apoll_Type.tp_init = &Py_apoll::init_func;
    if (PyType_Ready(&apoll_Type) < 0) {
        return;
    }

    PyObject *m = Py_InitModule("pyasynchio", pyasynchio_methods);
    Py_INCREF(&apoll_Type);
    PyModule_AddObject(m
        , "apoll"
        , reinterpret_cast<PyObject*>(&apoll_Type) );
}


} // namespace pyasynchio