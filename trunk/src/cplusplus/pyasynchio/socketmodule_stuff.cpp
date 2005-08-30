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


#include <pyasynchio/socketmodule_stuff.hpp>

namespace pyasynchio {

#ifdef USE_GETADDRINFO_LOCK
#define ACQUIRE_GETADDRINFO_LOCK PyThread_acquire_lock(netdb_lock, 1);
#define RELEASE_GETADDRINFO_LOCK PyThread_release_lock(netdb_lock);
#else
#define ACQUIRE_GETADDRINFO_LOCK
#define RELEASE_GETADDRINFO_LOCK
#endif


static PyObject *
    set_gaierror(int error)
{
    PyObject *v;

#ifdef EAI_SYSTEM
    /* EAI_SYSTEM is not available on Windows XP. */
    if (error == EAI_SYSTEM)
        return set_error();
#endif

#ifdef HAVE_GAI_STRERROR
    v = Py_BuildValue("(is)", error, gai_strerror(error));
#else
    v = Py_BuildValue("(is)", error, "getaddrinfo failed");
#endif
    if (v != NULL) {
        PyErr_SetObject(PyExc_RuntimeError, v); // gaierror should be here
        Py_DECREF(v);
    }

    return NULL;
}


/* Create a string object representing an IP address.
This is always a string of the form 'dd.dd.dd.dd' (with variable
size numbers). */

PyObject * makeipaddr(struct sockaddr *addr, int addrlen)
{
    char buf[NI_MAXHOST];
    int error;

    error = getnameinfo(addr, addrlen, buf, sizeof(buf), NULL, 0,
        NI_NUMERICHOST);
    if (error) {
        set_gaierror(error);
        return NULL;
    }
    return PyString_FromString(buf);
}

/* Create an object representing the given socket address,
suitable for passing it back to bind(), connect() etc.
The family field of the sockaddr structure is inspected
to determine what kind of address it really is. */

/*ARGSUSED*/
PyObject * makesockaddr(int sockfd, struct sockaddr *addr, int addrlen, int proto)
{
    if (addrlen == 0) {
        /* No address -- may be recvfrom() from known socket */
        Py_INCREF(Py_None);
        return Py_None;
    }

#ifdef __BEOS__
    /* XXX: BeOS version of accept() doesn't set family correctly */
    addr->sa_family = AF_INET;
#endif

    switch (addr->sa_family) {

    case AF_INET:
        {
            struct sockaddr_in *a;
            PyObject *addrobj = makeipaddr(addr, sizeof(*a));
            PyObject *ret = NULL;
            if (addrobj) {
                a = (struct sockaddr_in *)addr;
                ret = Py_BuildValue("Oi", addrobj, ntohs(a->sin_port));
                Py_DECREF(addrobj);
            }
            return ret;
        }

#if defined(AF_UNIX)
    case AF_UNIX:
        {
            struct sockaddr_un *a = (struct sockaddr_un *) addr;
            return PyString_FromString(a->sun_path);
        }
#endif /* AF_UNIX */

#ifdef ENABLE_IPV6
    case AF_INET6:
        {
            struct sockaddr_in6 *a;
            PyObject *addrobj = makeipaddr(addr, sizeof(*a));
            PyObject *ret = NULL;
            if (addrobj) {
                a = (struct sockaddr_in6 *)addr;
                ret = Py_BuildValue("Oiii",
                    addrobj,
                    ntohs(a->sin6_port),
                    a->sin6_flowinfo,
                    a->sin6_scope_id);
                Py_DECREF(addrobj);
            }
            return ret;
        }
#endif

#ifdef USE_BLUETOOTH
    case AF_BLUETOOTH:
        switch (proto) {

    case BTPROTO_L2CAP:
        {
            struct sockaddr_l2 *a = (struct sockaddr_l2 *) addr;
            PyObject *addrobj = makebdaddr(&_BT_L2_MEMB(a, bdaddr));
            PyObject *ret = NULL;
            if (addrobj) {
                ret = Py_BuildValue("Oi",
                    addrobj,
                    _BT_L2_MEMB(a, psm));
                Py_DECREF(addrobj);
            }
            return ret;
        }

    case BTPROTO_RFCOMM:
        {
            struct sockaddr_rc *a = (struct sockaddr_rc *) addr;
            PyObject *addrobj = makebdaddr(&_BT_RC_MEMB(a, bdaddr));
            PyObject *ret = NULL;
            if (addrobj) {
                ret = Py_BuildValue("Oi",
                    addrobj,
                    _BT_RC_MEMB(a, channel));
                Py_DECREF(addrobj);
            }
            return ret;
        }

#if !defined(__FreeBSD__)
    case BTPROTO_SCO:
        {
            struct sockaddr_sco *a = (struct sockaddr_sco *) addr;
            return makebdaddr(&_BT_SCO_MEMB(a, bdaddr));
        }
#endif

        }
#endif

#ifdef HAVE_NETPACKET_PACKET_H
    case AF_PACKET:
        {
            struct sockaddr_ll *a = (struct sockaddr_ll *)addr;
            char *ifname = "";
            struct ifreq ifr;
            /* need to look up interface name give index */
            if (a->sll_ifindex) {
                ifr.ifr_ifindex = a->sll_ifindex;
                if (ioctl(sockfd, SIOCGIFNAME, &ifr) == 0)
                    ifname = ifr.ifr_name;
            }
            return Py_BuildValue("shbhs#",
                ifname,
                ntohs(a->sll_protocol),
                a->sll_pkttype,
                a->sll_hatype,
                a->sll_addr,
                a->sll_halen);
        }
#endif

        /* More cases here... */

    default:
        /* If we don't know the address family, don't raise an
        exception -- return it as a tuple. */
        return Py_BuildValue("is#",
            addr->sa_family,
            addr->sa_data,
            sizeof(addr->sa_data));

    }
}

/* Convert a string specifying a host name or one of a few symbolic
names to a numeric IP address.  This usually calls gethostbyname()
to do the work; the names "" and "<broadcast>" are special.
Return the length (IPv4 should be 4 bytes), or negative if
an error occurred; then an exception is raised. */

static int
setipaddr(char *name, struct sockaddr *addr_ret, size_t addr_ret_size, int af)
{
    struct addrinfo hints, *res;
    int error;
    int d1, d2, d3, d4;
    char ch;

    memset((void *) addr_ret, '\0', sizeof(*addr_ret));
    if (name[0] == '\0') {
        int siz;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = af;
        hints.ai_socktype = SOCK_DGRAM; /*dummy*/
        hints.ai_flags = AI_PASSIVE;
        Py_BEGIN_ALLOW_THREADS
            ACQUIRE_GETADDRINFO_LOCK
            error = getaddrinfo(NULL, "0", &hints, &res);
        Py_END_ALLOW_THREADS
            /* We assume that those thread-unsafe getaddrinfo() versions
            *are* safe regarding their return value, ie. that a
            subsequent call to getaddrinfo() does not destroy the
            outcome of the first call. */
            RELEASE_GETADDRINFO_LOCK
            if (error) {
                set_gaierror(error);
                return -1;
            }
            switch (res->ai_family) {
        case AF_INET:
            siz = 4;
            break;
#ifdef ENABLE_IPV6
        case AF_INET6:
            siz = 16;
            break;
#endif
        default:
            freeaddrinfo(res);
            PyErr_SetString(PySocketModule.error,
                "unsupported address family");
            return -1;
            }
            if (res->ai_next) {
                freeaddrinfo(res);
                PyErr_SetString(PySocketModule.error,
                    "wildcard resolved to multiple address");
                return -1;
            }
            if (res->ai_addrlen < addr_ret_size)
                addr_ret_size = res->ai_addrlen;
            memcpy(addr_ret, res->ai_addr, addr_ret_size);
            freeaddrinfo(res);
            return siz;
    }
    if (name[0] == '<' && strcmp(name, "<broadcast>") == 0) {
        struct sockaddr_in *sin;
        if (af != AF_INET && af != AF_UNSPEC) {
            PyErr_SetString(PySocketModule.error,
                "address family mismatched");
            return -1;
        }
        sin = (struct sockaddr_in *)addr_ret;
        memset((void *) sin, '\0', sizeof(*sin));
        sin->sin_family = AF_INET;
#ifdef HAVE_SOCKADDR_SA_LEN
        sin->sin_len = sizeof(*sin);
#endif
        sin->sin_addr.s_addr = INADDR_BROADCAST;
        return sizeof(sin->sin_addr);
    }
    if (sscanf(name, "%d.%d.%d.%d%c", &d1, &d2, &d3, &d4, &ch) == 4 &&
        0 <= d1 && d1 <= 255 && 0 <= d2 && d2 <= 255 &&
        0 <= d3 && d3 <= 255 && 0 <= d4 && d4 <= 255) {
            struct sockaddr_in *sin;
            sin = (struct sockaddr_in *)addr_ret;
            sin->sin_addr.s_addr = htonl(
                ((long) d1 << 24) | ((long) d2 << 16) |
                ((long) d3 << 8) | ((long) d4 << 0));
            sin->sin_family = AF_INET;
#ifdef HAVE_SOCKADDR_SA_LEN
            sin->sin_len = sizeof(*sin);
#endif
            return 4;
        }
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = af;
        Py_BEGIN_ALLOW_THREADS
            ACQUIRE_GETADDRINFO_LOCK
            error = getaddrinfo(name, NULL, &hints, &res);
#if defined(__digital__) && defined(__unix__)
        if (error == EAI_NONAME && af == AF_UNSPEC) {
            /* On Tru64 V5.1, numeric-to-addr conversion fails
            if no address family is given. Assume IPv4 for now.*/
            hints.ai_family = AF_INET;
            error = getaddrinfo(name, NULL, &hints, &res);
        }
#endif
        Py_END_ALLOW_THREADS
            RELEASE_GETADDRINFO_LOCK  /* see comment in setipaddr() */
            if (error) {
                set_gaierror(error);
                return -1;
            }
            if (res->ai_addrlen < addr_ret_size)
                addr_ret_size = res->ai_addrlen;
            memcpy((char *) addr_ret, res->ai_addr, addr_ret_size);
            freeaddrinfo(res);
            switch (addr_ret->sa_family) {
    case AF_INET:
        return 4;
#ifdef ENABLE_IPV6
    case AF_INET6:
        return 16;
#endif
    default:
        PyErr_SetString(PySocketModule.error, "unknown address family");
        return -1;
            }
}


/* Parse a socket address argument according to the socket object's
address family.  Return 1 if the address was in the proper format,
0 of not.  The address is returned through addr_ret, its length
through len_ret. */

int getsockaddrarg(PySocketSockObject *s, PyObject *args,
                    struct sockaddr *addrp, int *len_ret)
{
    switch (s->sock_family) {

#if defined(AF_UNIX)
case AF_UNIX:
    {
        struct sockaddr_un* addr;
        char *path;
        int len;
        addr = (struct sockaddr_un*)addrp;
        if (!PyArg_Parse(args, "t#", &path, &len))
            return 0;
        if (len > sizeof addr->sun_path) {
            PyErr_SetString(socket_error,
                "AF_UNIX path too long");
            return 0;
        }
        addr->sun_family = s->sock_family;
        memcpy(addr->sun_path, path, len);
        addr->sun_path[len] = 0;
//      *addr_ret = (struct sockaddr *) addr;
#if defined(PYOS_OS2)
        *len_ret = sizeof(*addr);
#else
        *len_ret = len + sizeof(*addr) - sizeof(addr->sun_path);
#endif
        return 1;
    }
#endif /* AF_UNIX */

case AF_INET:
    {
        struct sockaddr_in* addr;
        char *host;
        int port, result;
        addr=(struct sockaddr_in*)addrp;
        if (!PyTuple_Check(args)) {
            PyErr_Format(
                PyExc_TypeError,
                "getsockaddrarg: "
                "AF_INET address must be tuple, not %.500s",
                args->ob_type->tp_name);
            return 0;
        }
        if (!PyArg_ParseTuple(args, "eti:getsockaddrarg", 
            "idna", &host, &port))
            return 0;
        result = setipaddr(host, (struct sockaddr *)addr, 
            sizeof(*addr),  AF_INET);
        PyMem_Free(host);
        if (result < 0)
            return 0;
        addr->sin_family = AF_INET;
        addr->sin_port = htons((short)port);
//      *addr_ret = (struct sockaddr *) addr;
        *len_ret = sizeof *addr;
        return 1;
    }

#ifdef ENABLE_IPV6
case AF_INET6:
    {
        struct sockaddr_in6* addr;
        char *host;
        int port, flowinfo, scope_id, result;
        addr = (struct sockaddr_in6*)addrp;
        flowinfo = scope_id = 0;
        if (!PyArg_ParseTuple(args, "eti|ii", 
            "idna", &host, &port, &flowinfo,
            &scope_id)) {
                return 0;
            }
            result = setipaddr(host, (struct sockaddr *)addr,  
                sizeof(*addr), AF_INET6);
            PyMem_Free(host);
            if (result < 0)
                return 0;
            addr->sin6_family = s->sock_family;
            addr->sin6_port = htons((short)port);
            addr->sin6_flowinfo = flowinfo;
            addr->sin6_scope_id = scope_id;
            *len_ret = sizeof *addr;
            return 1;
    }
#endif

#ifdef USE_BLUETOOTH
case AF_BLUETOOTH:
    {
        switch (s->sock_proto) {
case BTPROTO_L2CAP:
    {
        struct sockaddr_l2 *addr = (struct sockaddr_l2 *) addrp;
        char *straddr;

        _BT_L2_MEMB(addr, family) = AF_BLUETOOTH;
        if (!PyArg_ParseTuple(args, "si", &straddr,
            &_BT_L2_MEMB(addr, psm))) {
                PyErr_SetString(socket_error, "getsockaddrarg: "
                    "wrong format");
                return 0;
            }
            if (setbdaddr(straddr, &_BT_L2_MEMB(addr, bdaddr)) < 0)
                return 0;

            *len_ret = sizeof *addr;
            return 1;
    }
case BTPROTO_RFCOMM:
    {
        struct sockaddr_rc *addr = (struct sockaddr_rc *) addrp;
        char *straddr;

        _BT_RC_MEMB(addr, family) = AF_BLUETOOTH;
        if (!PyArg_ParseTuple(args, "si", &straddr,
            &_BT_RC_MEMB(addr, channel))) {
                PyErr_SetString(socket_error, "getsockaddrarg: "
                    "wrong format");
                return 0;
            }
            if (setbdaddr(straddr, &_BT_RC_MEMB(addr, bdaddr)) < 0)
                return 0;

            *addr_ret = (struct sockaddr *) addr;
            *len_ret = sizeof *addr;
            return 1;
    }
#if !defined(__FreeBSD__)
case BTPROTO_SCO:
    {
        struct sockaddr_sco *addr = (struct sockaddr_sco *) addrp;
        char *straddr;

        _BT_SCO_MEMB(addr, family) = AF_BLUETOOTH;
        straddr = PyString_AsString(args);
        if (straddr == NULL) {
            PyErr_SetString(socket_error, "getsockaddrarg: "
                "wrong format");
            return 0;
        }
        if (setbdaddr(straddr, &_BT_SCO_MEMB(addr, bdaddr)) < 0)
            return 0;

        *len_ret = sizeof *addr;
        return 1;
    }
#endif
default:
    PyErr_SetString(socket_error, "getsockaddrarg: unknown Bluetooth protocol");
    return 0;
        }
    }
#endif

#ifdef HAVE_NETPACKET_PACKET_H
case AF_PACKET:
    {
        struct sockaddr_ll* addr;
        struct ifreq ifr;
        char *interfaceName;
        int protoNumber;
        int hatype = 0;
        int pkttype = 0;
        char *haddr = NULL;
        unsigned int halen = 0;

        if (!PyArg_ParseTuple(args, "si|iis#", &interfaceName,
            &protoNumber, &pkttype, &hatype,
            &haddr, &halen))
            return 0;
        strncpy(ifr.ifr_name, interfaceName, sizeof(ifr.ifr_name));
        ifr.ifr_name[(sizeof(ifr.ifr_name))-1] = '\0';
        if (ioctl(s->sock_fd, SIOCGIFINDEX, &ifr) < 0) {
            s->errorhandler();
            return 0;
        }
        addr = (sockaddr_ll*)addrp;
        addr->sll_family = AF_PACKET;
        addr->sll_protocol = htons((short)protoNumber);
        addr->sll_ifindex = ifr.ifr_ifindex;
        addr->sll_pkttype = pkttype;
        addr->sll_hatype = hatype;
        if (halen > 8) {
            PyErr_SetString(PyExc_ValueError,
                "Hardware address must be 8 bytes or less");
            return 0;
        }
        if (halen != 0) {
            memcpy(&addr->sll_addr, haddr, halen);
        }
        addr->sll_halen = halen;
        *len_ret = sizeof *addr;
        return 1;
    }
#endif

    /* More cases here... */

default:
    PyErr_SetString(PySocketModule.error, "getsockaddrarg: bad family");
    return 0;

    }
}




} // namespace pyasynchio