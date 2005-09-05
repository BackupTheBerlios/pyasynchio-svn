#include <pyasynchio/win/apoll_impl.hpp>
#include <mswsock.h>

namespace pyasynchio {

namespace win {

apoll_impl::g_file_closers_type apoll_impl::g_file_closers;
apoll_impl::g_file_registry_type apoll_impl::g_file_registry;


apoll_impl::apoll_impl(::PyObject *args, ::PyObject *kwargs)
{
    unsigned long max_threads = 0;
	char *keywords[] = { "max_threads", 0 };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs
		, "|k:apoll"
		, keywords,
		&max_threads)) 
	{
		throw std::runtime_error("apoll_impl failed to initialize");
    }

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

apoll_impl::~apoll_impl()
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
        aop_root *ovr = 0;
        ovr = static_cast<aop_root*>(ovl);

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

bool apoll_impl::accept_impl(PySocketSockObject *lsock
							 , PySocketSockObject *asock
							 , PyObject *lsock_ref
							 , PyObject *asock_ref
							 , aop_accept *asynch_accept_op)
{
    if (!sock_iocp_preamble(lsock->sock_fd)) {
        return false;
    }
    DWORD num_bytes_rcvd = 0;
    BOOL r = ::AcceptEx(lsock->sock_fd    // sListenSocket 
        , asock->sock_fd                  // sAcceptSocket 
        , &asynch_accept_op->addr_buf_[0]                        // lpOutputBuffer
        , 0                                     // dwReceiveDataLength
        , asynch_accept_op->addr_size                             // dwLocalAddressLength
        , asynch_accept_op->addr_size                             // dwRemoteAddressLength
        , &num_bytes_rcvd                       // lpdwBytesReceived
        , asynch_accept_op   // lpOverlapped                     
        );                                  
    return check_wsa_op(r, TRUE, "::AcceptEx failed");
}

bool apoll_impl::connect_impl(::PySocketSockObject *so
							  , ::PyObject *so_ref
							  , sockaddr &addr
							  , int addr_len
							  , aop_connect *asynch_connect_op)
{
    if (!sock_iocp_preamble(so->sock_fd)) {
        return false;
    }
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
        , asynch_connect_op                     // lpOverlapped
        );
    return check_wsa_op(r, TRUE, "::ConnectEx failed");

}


bool apoll_impl::common_iocp_preamble(HANDLE h)
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

bool apoll_impl::sock_iocp_preamble(SOCKET sock)
{
    return common_iocp_preamble(reinterpret_cast<HANDLE>(sock));
}

HANDLE apoll_impl::get_file_handle(PyFileObject *fo)
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

        Py_BEGIN_ALLOW_THREADS;

        fh = ::CreateFileW(file_path.c_str()        // LPCTSTR lpFileName
            , desired_access                                    // DWORD dwDesiredAccess
            , share_mode                                        // DWORD dwShareMode
            , 0                                                 // LPSECURITY_ATTRIBUTES lpSecurityAttributes
            , OPEN_EXISTING                                     // DWORD dwCreationDisposition
            , FILE_FLAG_OVERLAPPED                              // DWORD dwFlagsAndAttributes
            , NULL);                                            // HANDLE hTemplateFile
        Py_END_ALLOW_THREADS;

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

HANDLE apoll_impl::file_iocp_preamble(PyFileObject *fo)
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

void apoll_impl::free_file_resources(FILE *fp)
{
    if (asynch_handles_.find(fp) != asynch_handles_.end()) {
        ::CloseHandle(asynch_handles_[fp]);
        asynch_handles_.erase(fp);
    }
}

int apoll_impl::g_fclose(FILE *fp)
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


std::wstring apoll_impl::get_path_by_handle(HANDLE h)
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

} // namespace win

} // namespace pyasynchio