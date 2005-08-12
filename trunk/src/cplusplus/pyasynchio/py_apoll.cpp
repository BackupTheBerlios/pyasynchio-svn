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

bool Py_apoll::preamble(HANDLE h)
{
	HANDLE iocp_ret = ::CreateIoCompletionPort(	h
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
	if (!preamble(reinterpret_cast<HANDLE>(listen_sock->sock_fd))) {
		return false;
	}
	DWORD num_bytes_rcvd = 0;
	OVL_ACCEPT *ova = new OVL_ACCEPT(acto				// acto
		, listen_sock									// lso
		, accept_sock		// aso
		);
	BOOL r = ::AcceptEx(listen_sock->sock_fd	// sListenSocket 
		, accept_sock->sock_fd					// sAcceptSocket 
		, &ova->addr_buf_[0]						// lpOutputBuffer
		, 0										// dwReceiveDataLength
		, addr_size								// dwLocalAddressLength
		, addr_size								// dwRemoteAddressLength
		, &num_bytes_rcvd						// lpdwBytesReceived
		, ova	// lpOverlapped						
		);									
	if (TRUE == r) {
		// OVERLAPPED will still get queued
		return true;
	}
	else {
		int wsa_error = WSAGetLastError();
		if (wsa_error == ERROR_IO_PENDING) {
			return true;
		}
		else {
			::PyErr_SetString(PySocketModule.error, "::AcceptEx failed");
			return false;
		}
	}
	return false;
}

bool Py_apoll::connect(::PySocketSockObject *so, ::PyObject *addro, ::PyObject *acto)
{
	if (!preamble(reinterpret_cast<HANDLE>(so->sock_fd))) {
		return false;
	}
	sockaddr * addr = 0;
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
	BOOL r = ConnectEx(so->sock_fd			// s
		, addr									// name
		, addr_len								// namelen
		, 0										// lpSendBuffer
		, 0										// dwSendDataLength
		, &bytes_sent							// lpdwBytesSent
		, ovc									// lpOverlapped
		);
	if (TRUE == r) {
		// notification still will be posted
		return true;
	}
	else {
		int wsa_error = WSAGetLastError();
		if (wsa_error == ERROR_IO_PENDING) {
			return true;
		}
		else {
			::PyErr_SetString(PySocketModule.error, "::ConnectEx failed");
			return false;
		}
	}
	return false;
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
		success = ::GetQueuedCompletionStatus(iocp_handle_	// CompletionPort
			, &bytes_transferred								// lpNumberOfBytesTransferred
			, &completion_key									// lpCompletionKey
			, &ovl												// lpOverlapped
			, ms												// dwMilliseconds
			);
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
	::GetAcceptExSockaddrs(addr_buf_		// lpOutputBuffer
		, 0									// dwReceiveDataLength
		, addr_size							// dwLocalAddressLength
		, addr_size							// dwRemoteAddressLength
		, &local_addr						// LocalSockaddr
		, &local_size						// LocalSockadddrLength
		, &remote_addr						// RemoteSockaddr
		, &remote_size						// RemoteSockaddrLength
		);					

	PyObject *local_addro = makesockaddr(
		static_cast<int>(lso_->sock_fd)					// sockfd
		, local_addr									// addr
		, local_size									// addrlen
		, lso_->sock_proto								// proto
		);
	
	PyObject *remote_addro = makesockaddr(
		static_cast<int>(aso_->sock_fd)					// sockfd
		, remote_addr									// addr
		, remote_size									// addrlen
		, aso_->sock_proto								// proto
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

PyObject* Py_apoll::connect_meth(Py_apoll *self, ::PyObject *args)
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

PyObject* Py_apoll::poll_meth(Py_apoll *self, ::PyObject *args)
{
	unsigned long ms;
	if (!PyArg_ParseTuple(args, "k", &ms)) {
		return NULL;
	}

	return self->poll(ms);
}

static PyMethodDef pyasynchio_methods[] = {
	{NULL}	/* Sentinel */
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
	, { "poll"
		, reinterpret_cast<PyCFunction>(&Py_apoll::poll_meth)
		, METH_VARARGS
		, "poll asynchronous operation results"
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