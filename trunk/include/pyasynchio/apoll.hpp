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

#ifndef PYASYNCHIO_APOLL_HPP_INCLUDED_
#define PYASYNCHIO_APOLL_HPP_INCLUDED_

#pragma once

#include <pyconfig.h>
#include <python.h>
#include "socketmodule.h"
#include "fileobject.h"
#include "fileobject.h"
#include <windows.h>
#include <map>


namespace pyasynchio {

extern ::PyTypeObject apoll_Type;

class Py_apoll : public PyObject
{
public:
    Py_apoll(unsigned long maxProcessingThreads = 0);
    ~Py_apoll();

    bool accept(::PySocketSockObject *listen_socket
        , ::PySocketSockObject *accept_socket
        , ::PyObject *lsock_ref_obj
        , ::PyObject *asock_ref_obj
        , ::PyObject *act);
    bool connect(::PySocketSockObject *so, ::PyObject *so_ref
        , ::PyObject *addro, ::PyObject *acto);
    bool send(::PySocketSockObject *so, ::PyObject *so_ref
        , ::PyObject *datao, unsigned long flags
        , ::PyObject *acto);
    bool sendto(::PySocketSockObject *so, ::PyObject *so_ref
        , ::PyObject *addro
        , ::PyObject *datao
        , unsigned long flags
        , ::PyObject *acto);
    bool recv(::PySocketSockObject *so, ::PyObject *so_ref
        , unsigned long bufsize
        , unsigned long flags
        , ::PyObject *acto);
    bool recvfrom(::PySocketSockObject *so, ::PyObject *so_ref
        , unsigned long bufsize
        , unsigned long flags
        , ::PyObject *acto);
    bool cancel(::PyObject *o);
    void close(::PyObject *o);

    bool read(::PyFileObject *fo, unsigned long long offset, unsigned long size
        , ::PyObject *acto);
    bool write(::PyFileObject *fo, unsigned long long offset
        , ::PyObject *datao
        , ::PyObject *acto);
        

    ::PyObject* poll(long ms);

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
    static ::PyObject * write_meth(Py_apoll *self, ::PyObject *args);

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

} // namespace pyasynchio

#endif // PYASYNCHIO_APOLL_HPP_INCLUDED_