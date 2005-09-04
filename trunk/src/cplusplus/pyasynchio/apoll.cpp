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


#include <pyasynchio/apoll.hpp>
#include <pyasynchio/utils.hpp>
#include <pyasynchio/aio.hpp>
#include <pyasynchio/socketmodule_stuff.hpp>
#include <new>
#include <internal.h>
#include <winternl.h>
#include <mswsock.h>
#include <string>

namespace pyasynchio {

Py_apoll::g_file_closers_type Py_apoll::g_file_closers;
Py_apoll::g_file_registry_type Py_apoll::g_file_registry;

Py_apoll::Py_apoll(unsigned long maxProcessingThreads /* = 0 */)
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
    for(;;) {
        BOOL success = FALSE;
        OVERLAPPED *ovl = NULL;
        ULONG bytes_transferred = 0;
        ULONG completion_key = 0;
        Py_BEGIN_ALLOW_THREADS;
        success = ::GetQueuedCompletionStatus(iocp_handle_  // CompletionPort
            , &bytes_transferred                                // lpNumberOfBytesTransferred
            , &completion_key                                   // lpCompletionKey
            , &ovl                                              // lpOverlapped
            , 0                                                // dwMilliseconds
            );
        Py_END_ALLOW_THREADS;
        AIO_ROOT *ovr = 0;
        ovr = static_cast<AIO_ROOT*>(ovl);

        if ( (0 == ovl) && (FALSE == success) ) {
            break;
        }

        delete ovr;
    }


    ::CloseHandle(iocp_handle_);
    asynch_handles_type::const_iterator citer;
    for(citer = asynch_handles_.begin(); citer != asynch_handles_.end(); ++citer) {
        std::pair<g_file_registry_type::iterator
            , g_file_registry_type::iterator> eqr = g_file_registry.equal_range(citer->first);
        g_file_registry_type::iterator fiter;
        for(fiter = eqr.first; fiter != eqr.second; ++fiter) {
            if (fiter->second == this) break;
        }
        g_file_registry.erase(fiter);
    }
    while(asynch_handles_.size() > 0) {
        this->free_file_resources(asynch_handles_.begin()->first);
    }
}

bool Py_apoll::common_iocp_preamble(HANDLE h)
{
    HANDLE iocp_ret = ::CreateIoCompletionPort(h
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

bool Py_apoll::sock_iocp_preamble(SOCKET sock)
{
    return common_iocp_preamble(reinterpret_cast<HANDLE>(sock));
}

bool Py_apoll::accept(::PySocketSockObject *listen_sock
                      , ::PySocketSockObject *accept_sock
                      , ::PyObject *lsock_ref_obj
                      , ::PyObject *asock_ref_obj
                      , ::PyObject *acto)
{
    if (!sock_iocp_preamble(listen_sock->sock_fd)) {
        return false;
    }
    AIO_ACCEPT *ova = new AIO_ACCEPT(acto               // acto
        , listen_sock                                   // lso
        , accept_sock       // aso
        , lsock_ref_obj
        , asock_ref_obj
        );
    DWORD num_bytes_rcvd = 0;
    BOOL r = ::AcceptEx(listen_sock->sock_fd    // sListenSocket 
        , accept_sock->sock_fd                  // sAcceptSocket 
        , &ova->addr_buf_[0]                        // lpOutputBuffer
        , 0                                     // dwReceiveDataLength
        , ova->addr_size                             // dwLocalAddressLength
        , ova->addr_size                             // dwRemoteAddressLength
        , &num_bytes_rcvd                       // lpdwBytesReceived
        , ova   // lpOverlapped                     
        );                                  
    return check_wsa_op(r, TRUE, "::AcceptEx failed");
}

bool Py_apoll::recv(::PySocketSockObject *so
                    , ::PyObject *so_ref
                    , unsigned long bufsize
                    , unsigned long flags
                    , ::PyObject *acto)
{
    if (!sock_iocp_preamble(so->sock_fd)) {
        return false;
    }
    AIO_RECV * ovr = new AIO_RECV(acto, so_ref, bufsize, flags);
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

bool Py_apoll::recvfrom(::PySocketSockObject * so
                        , ::PyObject *so_ref
                        , unsigned long bufsize
                        , unsigned long flags
                        , ::PyObject * acto)
{
    if (!sock_iocp_preamble(so->sock_fd)) {
        return false;
    }
    AIO_RECVFROM *ovf = new AIO_RECVFROM(acto, so_ref, so, bufsize, flags);
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

bool Py_apoll::sendto(::PySocketSockObject *so, ::PyObject *so_ref
                      , ::PyObject *addro
                      , ::PyObject *datao
                      , unsigned long flags
                      , ::PyObject *acto)
{
    if (!sock_iocp_preamble(so->sock_fd)) {
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

    AIO_SENDTO *ovt = new AIO_SENDTO(acto, so_ref, addro, flags, datao);

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

bool Py_apoll::send(::PySocketSockObject *so, ::PyObject *so_ref
                    , ::PyObject *datao, unsigned long flags
                    , ::PyObject *acto)
{
    if (!sock_iocp_preamble(so->sock_fd)) {
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
    AIO_SEND * ovs = new AIO_SEND(acto, so_ref, flags, datao);
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

bool Py_apoll::connect(::PySocketSockObject *so, ::PyObject *so_ref
                       , ::PyObject *addro, ::PyObject *acto)
{
    if (!sock_iocp_preamble(so->sock_fd)) {
        return false;
    }
    sockaddr addr;
    int addr_len = 0;
    if (!getsockaddrarg(so, addro, &addr, &addr_len)) {
        return false;
    }
    AIO_CONNECT * ovc = new AIO_CONNECT(acto, so_ref, addro);
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
        if (PyObject_IsInstance(o, reinterpret_cast<PyObject*>(&PyFile_Type))) {
            PyFileObject *fo = reinterpret_cast<PyFileObject*>(o);
            h = get_file_handle(fo);
        }
        else {
            PyErr_SetString(PyExc_TypeError, "unrecognized I/O object type");
            return false;
        }
    }
    if(! ::CancelIo(h)) {
        PyErr_SetString(PyExc_RuntimeError, "::CancelIo failed");
        return false;
    }
    return true;
}

static std::wstring get_path_by_handle(HANDLE h)
{
    typedef enum _OBJECT_INFORMATION_CLASS
    {
        ObjectBasicInformation,         // Result is OBJECT_BASIC_INFORMATION structure
        ObjectNameInformation,          // Result is OBJECT_NAME_INFORMATION structure
        ObjectTypeInformation,          // Result is OBJECT_TYPE_INFORMATION structure
        ObjectAllInformation,           // Result is OBJECT_ALL_INFORMATION structure
        ObjectDataInformation           // Result is OBJECT_DATA_INFORMATION structure
        
    } OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

    typedef LONG (__stdcall * NtQueryObjectT)(HANDLE, OBJECT_INFORMATION_CLASS, PVOID, ULONG, PULONG);
    NtQueryObjectT _NtQueryObject;
    _NtQueryObject = (NtQueryObjectT)GetProcAddress(LoadLibrary("ntdll.dll"), "NtQueryObject");
    typedef struct _FNAME {
        USHORT Length;
        USHORT MaximumLength;
        PWSTR  Buffer;
        WCHAR ObjectNameBuffer[1];
    } FNAME; 

    const unsigned int DEFAULT_BUFFER_SIZE = 10 * sizeof(WCHAR) * MAX_PATH;
    FNAME *fname = reinterpret_cast<FNAME*>(malloc(DEFAULT_BUFFER_SIZE));
    ULONG fname_length = DEFAULT_BUFFER_SIZE;
    
    _NtQueryObject(h, ObjectNameInformation, fname, DEFAULT_BUFFER_SIZE, &fname_length);

    std::wstring fname_str(fname->ObjectNameBuffer);
    free(fname);
    WCHAR drive_strings[32 * 4 + 1];

    if (GetLogicalDriveStringsW(sizeof(drive_strings)/sizeof(drive_strings[0])-1
        , drive_strings)) 
    {
        WCHAR szName[MAX_PATH];
        WCHAR szDrive[3] = L" :";
        BOOL bFound = FALSE;
        WCHAR* p = drive_strings;

        do 
        {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDeviceW(szDrive, szName, sizeof(szName) / sizeof(szName[0])))
            {
                size_t uNameLen = wcslen(szName);

                if (uNameLen < MAX_PATH) 
                {
                    bFound = _wcsnicmp(fname_str.c_str(), szName, 
                        uNameLen) == 0;

                    if (bFound) 
                    {
                        // Reconstruct pszFilename using szTemp
                        // Replace device path with DOS path
                        return fname_str.replace(0, uNameLen, szDrive);
                    }
                }
        }

        // Go to the next NULL character.
        while (*p++);
        } while (!bFound && *p); // end of string
    }
    return fname_str;
}

HANDLE Py_apoll::get_file_handle(PyFileObject *fo)
{
    FILE * fp = fo->f_fp;
    if (asynch_handles_.find(fp) == asynch_handles_.end()) {
        std::wstring file_path = get_path_by_handle((HANDLE)_get_osfhandle(_fileno(fp)));
        // create new handle.
        DWORD desired_access = 0;
        char * mode = PyString_AsString(fo->f_mode);
        if (mode[0] == 'r') {
            desired_access |= GENERIC_READ;
        }
        else if (mode[0] == 'w' || mode[0] == 'a') {
            desired_access |= GENERIC_WRITE;
        }
        DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        HANDLE fh = INVALID_HANDLE_VALUE;
//      if (PyUnicode_Check(fo->f_name)) {

        Py_BEGIN_ALLOW_THREADS;

        fh = ::CreateFileW(file_path.c_str()        // LPCTSTR lpFileName
            , desired_access                                    // DWORD dwDesiredAccess
            , share_mode                                        // DWORD dwShareMode
            , 0                                                 // LPSECURITY_ATTRIBUTES lpSecurityAttributes
            , OPEN_EXISTING                                     // DWORD dwCreationDisposition
            , FILE_FLAG_OVERLAPPED                              // DWORD dwFlagsAndAttributes
            , NULL);                                            // HANDLE hTemplateFile
        Py_END_ALLOW_THREADS;
//      }
        //else {
        //  Py_BEGIN_ALLOW_THREADS;
        //  char * fname = PyString_AS_STRING(fo->f_name);
        //  fh = ::CreateFileA(fname
        //      , desired_access
        //      , share_mode
        //      , 0
        //      , OPEN_EXISTING
        //      , FILE_FLAG_OVERLAPPED
        //      , NULL);
        //  Py_END_ALLOW_THREADS;
        //}
        if (fh == INVALID_HANDLE_VALUE) {
            PyErr_SetString(PyExc_RuntimeError, "CreateFile failed");
            return INVALID_HANDLE_VALUE;
        }
        asynch_handles_[fp] = fh;
        if (g_file_closers.find(fp) == g_file_closers.end()) {
            g_file_closers[fp] = fo->f_close;
            fo->f_close = &g_fclose;
        }
        g_file_registry.insert(std::make_pair(fp, this));

        
        return fh;
    }
    else {
        return asynch_handles_[fp];
    }
}

int Py_apoll::g_fclose(FILE *fp)
{
    std::pair<g_file_registry_type::const_iterator
        , g_file_registry_type::const_iterator> eqr = g_file_registry.equal_range(fp);
    g_file_registry_type::const_iterator citer;
    for( citer = eqr.first; citer != eqr.second; ++citer) {
        citer->second->free_file_resources(fp);
    }
    g_file_registry.erase(fp);
    int retval = g_file_closers[fp](fp);
    g_file_closers.erase(fp);
    return retval;
}

void Py_apoll::free_file_resources(FILE *fp)
{
    if (asynch_handles_.find(fp) != asynch_handles_.end()) {
        ::CloseHandle(asynch_handles_[fp]);
        asynch_handles_.erase(fp);
    }
}

HANDLE Py_apoll::file_iocp_preamble(PyFileObject *fo)
{
    HANDLE fh = get_file_handle(fo);
    if (INVALID_HANDLE_VALUE == fh) {
        return INVALID_HANDLE_VALUE;
    }
    if (!common_iocp_preamble(fh)) {
        free_file_resources(fo->f_fp);
        g_file_registry.erase(fo->f_fp);
        return INVALID_HANDLE_VALUE;
    }
    return fh;
}

bool Py_apoll::read(PyFileObject *fo, unsigned long long offset, unsigned long size
                    , ::PyObject *acto)
{
    HANDLE fh = file_iocp_preamble(fo);
    if (fh == INVALID_HANDLE_VALUE) {
        return false;
    }

    AIO_READ * ar = new AIO_READ(acto, fo, offset, size);
    DWORD bytes_read = 0;
    BOOL r = ::ReadFile(fh, ar->buf(), size, &bytes_read, ar);
    return check_windows_op(r, FALSE, "::ReadFile failed");
}

bool Py_apoll::write(PyFileObject *fo, unsigned long long offset
                     , ::PyObject *datao, ::PyObject *acto)
{
    HANDLE fh = file_iocp_preamble(fo);
    if (fh == INVALID_HANDLE_VALUE) {
        return false;
    }

    int len;
    char *s;
    if(-1 == PyString_AsStringAndSize(datao, &s, &len)) {
        return NULL;
    }

    AIO_WRITE * wr = new AIO_WRITE(acto, fo, offset, datao);
    DWORD bytes_written = 0;
    BOOL r = ::WriteFile(fh, s, len, &bytes_written, wr);
    return check_windows_op(r, FALSE, "::WriteFile failed");

}

::PyObject * Py_apoll::poll(long ms)
{
    DWORD bytes_transferred = 0;
    ULONG_PTR completion_key = 0;
    AIO_ROOT *ovr = 0;
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
        ovr = static_cast<AIO_ROOT*>(ovl);

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

        PyObject *op_result = ovr->to_python(success, bytes_transferred);
        PyList_Append(result, op_result);
        Py_XDECREF(op_result);
        delete ovr;
    }

    return result;
}

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
    , { "read"
        , reinterpret_cast<PyCFunction>(&Py_apoll::read_meth)
        , METH_VARARGS
        , "start asynchronous read operation on file"
    }
    , { "write"
        , reinterpret_cast<PyCFunction>(&Py_apoll::write_meth)
        , METH_VARARGS
        , "start asynchronous write operation on file" 
    }
    , {NULL} /* Sentinel */
};

PyTypeObject apoll_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "_pyasynchio.apoll",             /*tp_name*/
    sizeof(Py_apoll), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Py_apoll::dealloc,                         /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "apoll objects",           /* tp_doc */

    0,                                  // tp_traverse 
    0,                                  // tp_clear
    0,                                  // tp_richcompare 
    0,                                  // tp_weaklistoffset 

    0,                                  // tp_iter
    0,                                  // tp_iternext 

    apoll_methods,                      // tp_methods 
    0,                                  // tp_members
    0,                                  // tp_getset
    0,                                  // tp_base 
    
    0,                                  // tp_dict
    0,                                  // tp_descr_get
    0,                                  // tp_descr_set
    0,                                  // tp_dictoffset 
    Py_apoll::init_func,               // tp_init
    0,                                  // tp_alloc 
    PyType_GenericNew,                 // tp_new
    0,                                  // tp_free
    0,                                  // tp_is_gc
    0,                                  // tp_bases
    0,                                  // tp_mro
    0,                                  // tp_cache
    0,                                  // tp_subclasses
    0,                                  // tp_weaklist
    0,                                  // tp_del
};


int Py_apoll::init_func(PyObject *self, PyObject *args, PyObject *kwds)
{
    unsigned long maxConcurrentThreads = 0;
    if (!PyArg_ParseTuple(args, "|k:apoll", &maxConcurrentThreads)) {
        return NULL;
    }
    new (self) pyasynchio::Py_apoll(maxConcurrentThreads);
    return TRUE;
}

void Py_apoll::dealloc(pyasynchio::Py_apoll *self)
{
    self->~Py_apoll();
    self->ob_type->tp_free(self);
}

PyObject* Py_apoll::accept_meth(Py_apoll *self, ::PyObject *args)
{
    PyObject *listen_sock_raw, *accept_sock_raw
        , *lsock_ref_obj, *asock_ref_obj
        , *act;
    if (!PyArg_ParseTuple(args, "OOOOO:accept", &listen_sock_raw, &accept_sock_raw
        , &lsock_ref_obj, &asock_ref_obj
        , &act)) {
        return NULL;
    }

    PySocketSockObject *listen_sock = py_convert<PySocketSockObject>(listen_sock_raw
        , socketmodule_api.Sock_Type);
    if (NULL == listen_sock) {
        return NULL;
    }

    PySocketSockObject *accept_sock = py_convert<PySocketSockObject>(accept_sock_raw
        , socketmodule_api.Sock_Type);
    if (NULL == accept_sock) {
        return NULL;
    }

    if(!self->accept(listen_sock, accept_sock, lsock_ref_obj, asock_ref_obj, act)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::connect_meth(Py_apoll *self, ::PyObject *args)
{
    PyObject *so_raw, *so_ref, *addro, *acto;
    if (!PyArg_ParseTuple(args, "OOOO:connect", &so_raw, &so_ref, &addro, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->connect(so, so_ref, addro, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::sendto_meth(Py_apoll * self, ::PyObject * args)
{
    ::PyObject *so_raw, *so_ref, *addro, *datao, *acto;
    unsigned long flags;
    if (!PyArg_ParseTuple(args, "OOOOkO:sendto", &so_raw, &so_ref, &addro, &datao, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->sendto(so, so_ref, addro, datao, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::send_meth(Py_apoll * self, ::PyObject * args)
{
    ::PyObject *so_raw, *so_ref, *datao, *acto;
    unsigned long flags;
    if (!PyArg_ParseTuple(args, "OOOkO:send", &so_raw, &so_ref
        , &datao, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->send(so, so_ref, datao, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::recv_meth(Py_apoll *self, ::PyObject *args)
{
    ::PyObject *so_raw, *so_ref, *acto;
    unsigned long flags, size;
    if (!PyArg_ParseTuple(args, "OOkkO:recv", &so_raw, &so_ref
        , &size, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->recv(so, so_ref, size, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::recvfrom_meth(Py_apoll *self, ::PyObject *args)
{
    ::PyObject *so_raw, *so_ref, *acto;
    unsigned long flags, size;
    if (!PyArg_ParseTuple(args, "OOkkO:recvfrom", &so_raw, &so_ref
        , &size, &flags, &acto)) {
        return NULL;
    }

    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , socketmodule_api.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    if (!self->recvfrom(so, so_ref, size, flags, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::poll_meth(Py_apoll *self, ::PyObject *args)
{
    long ms;
    if (!PyArg_ParseTuple(args, "l:poll", &ms)) {
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

::PyObject * Py_apoll::write_meth(Py_apoll * self, ::PyObject * args)
{
    ::PyObject * fo_raw, *acto, *datao;
    unsigned long long offset = 0;
    if(!PyArg_ParseTuple(args, "OKOO:write", &fo_raw, &offset, &datao, &acto)) {
        return NULL;
    }

    PyFileObject * fo = py_convert<PyFileObject>(fo_raw, &PyFile_Type);
    if(NULL == fo) {
        return NULL;
    }

    if(!self->write(fo, offset, datao, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

::PyObject * Py_apoll::read_meth(Py_apoll * self, ::PyObject * args)
{
    ::PyObject * fo_raw, *acto;
    unsigned long long offset = 0;
    unsigned long size = 0;
    if (!PyArg_ParseTuple(args, "OKkO:read", &fo_raw, &offset, &size, &acto)) {
        return NULL;
    }

    PyFileObject * fo = py_convert<PyFileObject>(fo_raw, &PyFile_Type);
    if (NULL == fo) {
        return NULL;
    }

    if (!self->read(fo, offset, size, acto)) {
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

} // namespace pyasynchio