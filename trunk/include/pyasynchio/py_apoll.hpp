#ifndef PYASYNCHIO_PY_APOLL_HPP_INCLUDED_
#define PYASYNCHIO_PY_APOLL_HPP_INCLUDED_

#pragma once

#include <pyasynchio/detail/config.hpp>
#include <python.h>
#include "socketmodule.h"
#include "fileobject.h"
#include <windows.h>

namespace pyasynchio {

template<typename CPP_TYPE, typename PYTHON_TYPE_OBJ_TYPE>
CPP_TYPE* py_convert(PyObject *what, PYTHON_TYPE_OBJ_TYPE *type_obj)
{
	if (!PyObject_IsInstance(what, reinterpret_cast<PyObject*>(type_obj))) {
		PyErr_Format(PyExc_TypeError, "not a %s object", type_obj->tp_name);
		return NULL;
	}
	return reinterpret_cast<CPP_TYPE*>(what);
}

class PYASYNCHIO_LINK_DECL Py_apoll : public PyObject
{
public:
	static const unsigned int addr_size = sizeof(sockaddr_in) + sizeof(sockaddr);
	struct APOLL_OVERLAPPED : OVERLAPPED
	{
		APOLL_OVERLAPPED() 
		{
			Internal = InternalHigh = Offset = OffsetHigh = 0;
			hEvent = 0;
		}
		virtual ~APOLL_OVERLAPPED() {}

		static const unsigned int addr_buf_size = 2 * addr_size;

		unsigned char addr_buf[addr_buf_size];
	};

    Py_apoll();
    ~Py_apoll();

    void accept(::PySocketSockObject *listen_socket
		, ::PySocketSockObject *accept_socket
		, ::PyObject *act);
    void connect(::PySocketSockObject *so, ::PyObject *addro, ::PyObject *acto);
    void send(::PySocketSockObject *so, ::PyObject *datao, unsigned long flags
        , ::PyObject *acto);
    void sendto(::PySocketSockObject *so, ::PyObject *addro
        , ::PyObject *datao, ::PyObject *acto);
    void recv(::PySocketSockObject *so, unsigned long size, unsigned long flags
        , ::PyObject *acto);
    void cancel(::PyObject *o);
    void close(::PyObject *o);

	::PyObject* poll(unsigned long ms);

	static int init_func(PyObject *self, PyObject *args, PyObject *kwds);
	static PyObject* accept_meth(Py_apoll *self, ::PyObject *args);

private:
    HANDLE iocp_handle_;
};


extern "C" PYASYNCHIO_LINK_DECL void initpyasynchio(void);

} // namespace pyasynchio

#endif // PYASYNCHIO_PY_APOLL_HPP_INCLUDED_