#ifndef PYASYNCHIO_PY_APOLL_AIO_HPP_INCLUDED_
#define PYASYNCHIO_PY_APOLL_AIO_HPP_INCLUDED_

#pragma once

#include <pyasynchio/py_apoll.hpp>

namespace pyasynchio {

class Py_apoll::AIO_ROOT : public OVERLAPPED
{
protected:
    ::PyObject *acto_;

public:
    AIO_ROOT(::PyObject *acto) 
		: acto_(acto)
    {
        Internal = InternalHigh = 0;
        Offset = OffsetHigh = 0;
        hEvent = 0;
        Py_XINCREF(acto);
    }
    virtual ~AIO_ROOT() 
    {
        Py_XDECREF(acto_);
    }

    virtual ::PyObject* dump(BOOL success, DWORD bytes_transferred) = 0;

};

class Py_apoll::AIO_ACCEPT : public Py_apoll::AIO_ROOT
{
public:
    AIO_ACCEPT(::PyObject *acto
        , ::PySocketSockObject *lso
        , ::PySocketSockObject *aso
		, ::PyObject *lso_ref
		, ::PyObject *aso_ref)
        : AIO_ROOT(acto) 
		, lso_ref_(lso_ref)
		, aso_ref_(aso_ref)
		, lfd_(lso->sock_fd)
		, afd_(aso->sock_fd)
		, lproto_(lso->sock_proto)
		, aproto_(aso->sock_proto)
    {
        Py_XINCREF(lso_ref);
        Py_XINCREF(aso_ref);
    }

    virtual ~AIO_ACCEPT() 
    {
        Py_XDECREF(lso_ref_);
        Py_XDECREF(aso_ref_);
    }

    virtual ::PyObject* dump(BOOL success, DWORD bytes_transferred);

    static const unsigned int addr_buf_size = 2 * addr_size;

    unsigned char addr_buf_[addr_buf_size];

private:
	SOCKET lfd_, afd_;
	int lproto_, aproto_;
    ::PyObject *lso_ref_;
    ::PyObject *aso_ref_;
};


class Py_apoll::AIO_CONNECT : public Py_apoll::AIO_ROOT
{
public:
	AIO_CONNECT(::PyObject *acto, ::PyObject *so_ref, ::PyObject *addro)
        : AIO_ROOT(acto)
		, so_ref_(so_ref)
		, addro_(addro)
    {
        Py_XINCREF(so_ref);
        Py_XINCREF(addro);
    }

    virtual ~AIO_CONNECT()
    {
        Py_XDECREF(so_ref_);
		Py_XDECREF(addro_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
private:
    ::PyObject *so_ref_;
	::PyObject *addro_;
};

class Py_apoll::AIO_RECV : public Py_apoll::AIO_ROOT
{
public:
    AIO_RECV(::PyObject *acto, ::PyObject *so_ref
        , unsigned long size
        , unsigned long flags)
        : AIO_ROOT(acto)
		, so_ref_(so_ref)
    {
        buf_ = reinterpret_cast<char*>(malloc(size));
        size_ = size;
        flags_ = flags;
        Py_XINCREF(so_ref);
    }

    virtual ~AIO_RECV()
    {
        Py_XDECREF(so_ref_);
        free(buf_);
    }

    virtual ::PyObject *dump(BOOL success, DWORD bytes_transferred);

    char * buf() const { return buf_; }

protected:
	::PyObject *so_ref_;
private:
    unsigned long size_, flags_;
    char * buf_;
};

class Py_apoll::AIO_RECVFROM : public Py_apoll::AIO_RECV
{
public:
    AIO_RECVFROM(::PyObject *acto, ::PyObject *so_ref
		, ::PySocketSockObject *so
        , unsigned long size
        , unsigned long flags)
        : AIO_RECV(acto, so_ref, size, flags)
		, fromlen_(sizeof(from_))
		, fd_(so->sock_fd)
		, proto_(so->sock_proto)
    {
    }

    virtual ~AIO_RECVFROM() {}

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);

    sockaddr * from() { return &from_; }
    int * fromlen() { return &fromlen_; }
private:
	SOCKET fd_;
	int proto_;
    sockaddr from_;
    int fromlen_;
};

class Py_apoll::AIO_SEND : public Py_apoll::AIO_ROOT
{
public:
    AIO_SEND(::PyObject *acto, ::PyObject *so_ref
        , unsigned long flags
        , ::PyObject *datao)
        : AIO_ROOT(acto)
		, so_ref_(so_ref)
		, datao_(datao)
		, flags_(flags)
    {
        Py_XINCREF(so_ref);
        Py_XINCREF(datao);
    }

    virtual ~AIO_SEND()
    {
        Py_XDECREF(so_ref_);
        Py_XDECREF(datao_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
private:
    ::PyObject *so_ref_;
    ::PyObject *datao_;
    unsigned long flags_;
};

class Py_apoll::AIO_SENDTO : public Py_apoll::AIO_SEND
{
public:
    AIO_SENDTO(::PyObject *acto, ::PyObject *so_ref
        , ::PyObject *addro
        , unsigned long flags
        , ::PyObject *datao)
        : AIO_SEND(acto, so_ref, flags, datao)
		, addro_(addro)
    {
        Py_XINCREF(addro);
    }

    virtual ~AIO_SENDTO()
    {
        Py_XDECREF(addro_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);

private:
    ::PyObject * addro_;
};

class Py_apoll::AIO_READ : public Py_apoll::AIO_ROOT
{
public:
    AIO_READ(::PyObject * acto, ::PyFileObject *fo
        , unsigned long long off
        , unsigned long size)
        : AIO_ROOT(acto)
    {
        Offset = static_cast<DWORD>(off);
        OffsetHigh = static_cast<DWORD>((off >> (sizeof(DWORD) * 8)));
        size_ = size;
        buf_ = reinterpret_cast<char*>(malloc(size));
        Py_XINCREF(fo);
        fo_ = fo;
    }

    virtual ~AIO_READ()
    {
        free(buf_);
        Py_XDECREF(fo_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
    char * buf() const { return buf_; }

private:
    ::PyFileObject * fo_;
    char * buf_;
    unsigned long size_;
};

class Py_apoll::AIO_WRITE : public Py_apoll::AIO_ROOT
{
public:
    AIO_WRITE(::PyObject *acto, ::PyFileObject *fo
        , unsigned long long off
        , ::PyObject *datao)
        : AIO_ROOT(acto)
    {
        Offset = static_cast<DWORD>(off);
        OffsetHigh = static_cast<DWORD>((off >> (sizeof(DWORD) * 8)));
        Py_XINCREF(fo);
        fo_ = fo;
        Py_XINCREF(datao);
        datao_ = datao;
    }

    virtual ~AIO_WRITE()
    {
        Py_XDECREF(fo_);
        Py_XDECREF(datao_);
    }


    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
private:
    ::PyFileObject * fo_;
    ::PyObject * datao_;
};




}

#endif //  PYASYNCHIO_PY_APOLL_AIO_HPP_INCLUDED_