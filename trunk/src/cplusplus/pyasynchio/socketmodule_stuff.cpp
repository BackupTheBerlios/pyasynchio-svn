#include <pyasynchio/socketmodule_stuff.hpp>

namespace pyasynchio {

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



} // namespace pyasynchio