#include <pyasynchio/py_apoll.hpp>
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

void Py_apoll::accept(::PySocketSockObject *listen_sock
					  , ::PySocketSockObject *accept_sock
					  , ::PyObject *act)
{
	HANDLE iocp_ret = ::CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(listen_sock->sock_fd)
		, iocp_handle_
		, NULL
		, 0);
	if (iocp_handle_ != iocp_ret) {
		::PyErr_Format(PyExc_RuntimeError
			, "Failed to associate I/O completion port with socket handle %d"
			, GetLastError());
		return;
	}

	
	DWORD num_bytes_rcvd = 0;
	APOLL_OVERLAPPED *ao = new APOLL_OVERLAPPED();
	BOOL r = ::AcceptEx(listen_sock->sock_fd
		, accept_sock->sock_fd
		, &ao->addr_buf[0]
		, 0
		, addr_size
		, addr_size
		, &num_bytes_rcvd
		, ao);
	if (TRUE == r) {
		::PyErr_SetString(PyExc_RuntimeError, "operation completed synchronously");
		return;
	}
	else {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			::PyErr_SetString(PyExc_RuntimeError, "something wrong happened");
			return;
		}
		::PyErr_SetString(PyExc_RuntimeError, "operation completed asynchronously");
	}
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

static PyMethodDef apoll_methods[] = {
	{ "accept"
		, reinterpret_cast<PyCFunction>(&Py_apoll::accept_meth)
		, METH_VARARGS
		, "start asynchronous accept operation"
	}
	, {NULL} /* Sentinel */
};

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

	self->accept(listen_sock, accept_sock, act);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef pyasynchio_methods[] = {
	{NULL}	/* Sentinel */
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