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
#include "fileobject.h"
#include <windows.h>
#include <mswsock.h>
#include <pyasynchio/socketmodule_stuff.hpp>
#include <map>


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
	class AIO_ROOT;
	class AIO_ACCEPT;
	class AIO_CONNECT;
	class AIO_RECV;
	class AIO_RECVFROM;
	class AIO_SEND;
	class AIO_SENDTO;
	class AIO_READ;

    Py_apoll();
    ~Py_apoll();

    bool accept(::PySocketSockObject *listen_socket
		, ::PySocketSockObject *accept_socket
		, ::PyObject *act);
    bool connect(::PySocketSockObject *so, ::PyObject *addro, ::PyObject *acto);
    bool send(::PySocketSockObject *so, ::PyObject *datao, unsigned long flags
        , ::PyObject *acto);
    bool sendto(::PySocketSockObject *so, ::PyObject *addro
        , ::PyObject *datao
		, unsigned long flags
		, ::PyObject *acto);
    bool recv(::PySocketSockObject *so, unsigned long bufsize
		, unsigned long flags
        , ::PyObject *acto);
	bool recvfrom(::PySocketSockObject *so, unsigned long bufsize
		, unsigned long flags
		, ::PyObject *acto);
    bool cancel(::PyObject *o);
    void close(::PyObject *o);

	bool read(::PyFileObject *fo, unsigned long long offset, unsigned long size
		, ::PyObject *acto);
		

	::PyObject* poll(unsigned long ms);

	static int init_func(PyObject *self, PyObject *args, PyObject *kwds);
	static void dealloc(Py_apoll *self);
	static ::PyObject * accept_meth(Py_apoll *self, ::PyObject *args);
	static ::PyObject * connect_meth(Py_apoll *self, ::PyObject *args);
	static ::PyObject * send_meth(Py_apoll *self, ::PyObject *args);
	static ::PyObject * sendto_meth(Py_apoll *self, ::PyObject *args);
	static ::PyObject * recv_meth(Py_apoll *self, ::PyObject *args);
	static ::PyObject * recvfrom_meth(Py_apoll *self, ::PyObject *args);
	static ::PyObject * poll_meth(Py_apoll *self, ::PyObject *args);
	static ::PyObject * cancel_meth(Py_apoll *self, ::PyObject *args);

	static ::PyObject * read_meth(Py_apoll *self, ::PyObject *args);

private:
    HANDLE iocp_handle_;
	bool common_iocp_preamble(HANDLE h);
	bool sock_iocp_preamble(SOCKET sock);
	HANDLE file_iocp_preamble(::PyFileObject *fo);
	template<typename T>
	static bool check_wsa_op(T function_result, T no_error_result, char *msg);
	template<typename T>
	static bool check_windows_op(T function_result, T error_result, char *msg);

#pragma warning(disable:4251)
	typedef std::map<FILE *, HANDLE> asynch_handles_type;
	asynch_handles_type asynch_handles_;
#pragma warning(default:4251)
	HANDLE get_file_handle(::PyFileObject *fo);
	void free_file_resources(FILE * fp);

	static f_close(FILE * fp);

#pragma warning(disable:4251)
	typedef std::map<FILE *, int(*)(FILE*)> g_file_closers_type;
	static g_file_closers_type g_file_closers;
	typedef std::multimap<FILE *, Py_apoll *> g_file_registry_type;
	static g_file_registry_type g_file_registry;
#pragma warning(default:4251)
	static int g_fclose(FILE *fp);
};

template<typename T>
bool Py_apoll::check_wsa_op(T function_result, T no_error_result, char *msg)
{
	if (no_error_result == function_result) {
		// no error, notification is posted.
		return true;
	}
	else {
		int wsa_error = ::WSAGetLastError();
		if (WSA_IO_PENDING == wsa_error) {
			return true;
		}
		else {
			::PyErr_SetString(PySocketModule.error, msg);
			return false;
		}
	}
	return false;
}


template<typename T>
bool Py_apoll::check_windows_op(T function_result, T error_result, char *msg)
{
	if (error_result != function_result) {
		// no error, notification is posted.
		return true;
	}
	else {
		int err = ::GetLastError();
		if (ERROR_IO_PENDING == err) {
			return true;
		}
		else {
			::PyErr_SetString(PyExc_RuntimeError, msg);
			return false;
		}
	}
	return false;
}


extern "C" PYASYNCHIO_LINK_DECL void initpyasynchio(void);

} // namespace pyasynchio

#include <pyasynchio/py_apoll_aio.hpp>

#endif // PYASYNCHIO_PY_APOLL_HPP_INCLUDED_