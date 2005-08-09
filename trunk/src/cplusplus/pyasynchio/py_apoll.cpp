#include <pyasynchio/py_apoll.hpp>

namespace pyasynchio {

Py_apoll::Py_apoll()
{
	iocp_handle_ = ::CreateIoCompletionPort(
		INVALID_HANDLE_VALUE // File handle 
		, NULL // existing completion port
		, NULL // completion key
		, 0 // number of concurrent threads
		);
}

Py_apoll::~Py_apoll()
{
	::CloseHandle(iocp_handle_);
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

static PyMethodDef pyasynchio_methods[] = {
	{NULL}	/* Sentinel */
};

void initpyasynchio()
{
	apoll_Type.tp_new = PyType_GenericNew;
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