#ifndef PYASYNCHIO_WIN_APOLL_IMPL_HPP_INCLUDED_
#define PYASYNCHIO_WIN_APOLL_IMPL_HPP_INCLUDED_

#pragma once

#include <python.h>
#include "socketmodule.h"
#include <pyasynchio/aio.hpp>
#include <map>

namespace pyasynchio {

namespace win {

class apoll_impl
{
public:
    apoll_impl(PyObject * args, PyObject *kwargs);
    ~apoll_impl();

    bool accept_impl(::PySocketSockObject *lsock
        , ::PySocketSockObject *asock
        , ::PyObject *lsock_ref
        , ::PyObject *asock_ref
        , AIO_ACCEPT * asynch_accept_op);

protected:
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
    typedef std::multimap<FILE *, apoll_impl *> g_file_registry_type;
    static g_file_registry_type g_file_registry;
#pragma warning(default:4251)
    static int g_fclose(FILE *fp);

	static std::wstring get_path_by_handle(HANDLE h);
};

template<typename T>
bool apoll_impl::check_wsa_op(T function_result, T no_error_result, char *msg)
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
bool apoll_impl::check_windows_op(T function_result, T error_result, char *msg)
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


} // namespace win


} // namespace pyasynchio

#endif // PYASYNCHIO_WIN_APOLL_IMPL_HPP_INCLUDED_