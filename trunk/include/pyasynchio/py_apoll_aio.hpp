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
    {
        Internal = InternalHigh = 0;
        Offset = OffsetHigh = 0;
        hEvent = 0;
        Py_XINCREF(acto);
        acto_ = acto;
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
        , ::PySocketSockObject *aso)
        : AIO_ROOT(acto) 
    {
        Py_XINCREF(lso);
        lso_ = lso;
        Py_XINCREF(aso);
        aso_ = aso;
    }

    virtual ~AIO_ACCEPT() 
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


class Py_apoll::AIO_CONNECT : public Py_apoll::AIO_ROOT
{
public:
    AIO_CONNECT(::PyObject *acto, ::PySocketSockObject *so)
        : AIO_ROOT(acto)
    {
        Py_XINCREF(so);
        so_ = so;
    }

    virtual ~AIO_CONNECT()
    {
        Py_XDECREF(so_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
private:
    ::PySocketSockObject *so_;
};

class Py_apoll::AIO_RECV : public Py_apoll::AIO_ROOT
{
public:
    AIO_RECV(::PyObject *acto, ::PySocketSockObject *so
        , unsigned long size
        , unsigned long flags)
        : AIO_ROOT(acto)
    {
        buf_ = reinterpret_cast<char*>(malloc(size));
        size_ = size;
        flags_ = flags;
        Py_XINCREF(so);
        so_ = so;
    }

    virtual ~AIO_RECV()
    {
        Py_XDECREF(so_);
        free(buf_);
    }

    virtual ::PyObject *dump(BOOL success, DWORD bytes_transferred);

    char * buf() const { return buf_; }

protected:
    PySocketSockObject *so_;
private:
    unsigned long size_, flags_;
    char * buf_;
};

class Py_apoll::AIO_RECVFROM : public Py_apoll::AIO_RECV
{
public:
    AIO_RECVFROM(::PyObject *acto, ::PySocketSockObject *so
        , unsigned long size
        , unsigned long flags)
        : AIO_RECV(acto, so, size, flags)
    {
        fromlen_ = sizeof(from_);
    }

    virtual ~AIO_RECVFROM() {}

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);

    sockaddr * from() { return &from_; }
    int * fromlen() { return &fromlen_; }
private:
    sockaddr from_;
    int fromlen_;
};

class Py_apoll::AIO_SEND : public Py_apoll::AIO_ROOT
{
public:
    AIO_SEND(::PyObject *acto, ::PySocketSockObject *so
        , unsigned long flags
        , ::PyObject *datao)
        : AIO_ROOT(acto)
    {
        Py_XINCREF(so);
        so_ = so;
        Py_XINCREF(datao);
        datao_ = datao;
        flags_ = flags;
    }

    virtual ~AIO_SEND()
    {
        Py_XDECREF(so_);
        Py_XDECREF(datao_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
private:
    ::PySocketSockObject *so_;
    ::PyObject *datao_;
    unsigned long flags_;
};

class Py_apoll::AIO_SENDTO : public Py_apoll::AIO_SEND
{
public:
    AIO_SENDTO(::PyObject *acto, ::PySocketSockObject *so
        , ::PyObject *addro
        , unsigned long flags
        , ::PyObject *datao)
        : AIO_SEND(acto, so, flags, datao)
    {
        Py_XINCREF(addro);
        addro_ = addro;
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
		OffsetHigh = static_cast<DWORD>((off >> sizeof(DWORD) * 8));
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




}

#endif //  PYASYNCHIO_PY_APOLL_AIO_HPP_INCLUDED_