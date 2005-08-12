#ifndef PYASYNCHIO_PY_APOLL_HPP_INCLUDED_
#define PYASYNCHIO_PY_APOLL_HPP_INCLUDED_

#pragma once

#include <pyasynchio/detail/config.hpp>
#pragma push_macro("_DEBUG")
#undef _DEBUG
#include <pyconfig.h>
#pragma pop_macro("_DEBUG")
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
	class OVL_ROOT : public OVERLAPPED
	{
	protected:
		::PyObject *acto_;

	public:
		OVL_ROOT(::PyObject *acto) 
		{
			Internal = InternalHigh = 0;
			Offset = OffsetHigh = 0;
			hEvent = 0;
			Py_XINCREF(acto);
			acto_ = acto;
		}
		virtual ~OVL_ROOT() 
		{
			Py_XDECREF(acto_);
		}

		virtual ::PyObject* dump(BOOL success, DWORD bytes_transferred) = 0;

	};

	class OVL_ACCEPT : public OVL_ROOT
	{
	public:
		OVL_ACCEPT(::PyObject *acto
			, ::PySocketSockObject *lso
			, ::PySocketSockObject *aso)
			: OVL_ROOT(acto) 
		{
			Py_XINCREF(lso);
			lso_ = lso;
			Py_XINCREF(aso);
			aso_ = aso;
		}

		virtual ~OVL_ACCEPT() 
		{
			Py_XDECREF(lso_);
			Py_XDECREF(aso_);
		}

		virtual ::PyObject* dump(BOOL success, DWORD bytes_transferred);

		static const unsigned int addr_buf_size = 2 * addr_size;

		unsigned char addr_buf_[addr_buf_size];

	private:
		::PySocketSockObject *lso_;
		::PySocketSockObject *aso_;
	};


	class OVL_CONNECT : public OVL_ROOT
	{
	public:
		OVL_CONNECT(::PyObject *acto, ::PySocketSockObject *so)
			: OVL_ROOT(acto)
		{
			Py_XINCREF(so);
			so_ = so;
		}

		virtual ~OVL_CONNECT()
		{
			Py_XDECREF(so_);
		}

		virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
	private:
		::PySocketSockObject *so_;
	};

    Py_apoll();
    ~Py_apoll();

    bool accept(::PySocketSockObject *listen_socket
		, ::PySocketSockObject *accept_socket
		, ::PyObject *act);
    bool connect(::PySocketSockObject *so, ::PyObject *addro, ::PyObject *acto);
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
	static PyObject* connect_meth(Py_apoll *self, ::PyObject *args);
	static PyObject* poll_meth(Py_apoll *self, ::PyObject *args);

private:
    HANDLE iocp_handle_;
	bool preamble(HANDLE h);
};


extern "C" PYASYNCHIO_LINK_DECL void initpyasynchio(void);

} // namespace pyasynchio

#endif // PYASYNCHIO_PY_APOLL_HPP_INCLUDED_