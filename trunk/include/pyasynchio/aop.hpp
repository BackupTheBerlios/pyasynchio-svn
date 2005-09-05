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

#ifndef PYASYNCHIO_AOP_HPP_INCLUDED_
#define PYASYNCHIO_AOP_HPP_INCLUDED_

#pragma once

#include <pyasynchio/aioresult.hpp>
#include "socketmodule.h"
#include <windows.h>

namespace pyasynchio {

class aop_root : public OVERLAPPED
{
protected:
    ::PyObject *acto_;
	const char * name_;

public:
    aop_root(::PyObject *acto, const char * name) 
        : acto_(acto)
		, name_(name)
    {
        Internal = InternalHigh = 0;
        Offset = OffsetHigh = 0;
        hEvent = 0;
        Py_XINCREF(acto);
    }
    virtual ~aop_root() 
    {
        Py_XDECREF(acto_);
    }

    virtual ::PyObject* dump(BOOL success, DWORD bytes_transferred) = 0;

	::PyObject * to_python(BOOL success, DWORD bytes_transferred)
	{
		::PyObject * geno = aioresult::create();;
		{
			::PyObject * successo = PyBool_FromLong(success);
			::PyObject_SetAttrString(geno, "success", successo);
			Py_DECREF(successo);
		}
		{
			::PyObject * nameo = PyString_FromString(name_);
			::PyObject_SetAttrString(geno, "name", nameo);
			Py_DECREF(nameo);
		}
		::PyObject_SetAttrString(geno, "act", acto_);

		::PyObject * speco = this->dump(success, bytes_transferred);
		::PyObject * tp = Py_BuildValue("(OO)", geno, speco);
		Py_DECREF(geno);
		Py_DECREF(speco);
		return tp;
	}
};

class aop_accept : public aop_root
{
public:
    aop_accept(::PyObject *acto
        , ::PySocketSockObject *lso
        , ::PySocketSockObject *aso
        , ::PyObject *lso_ref
        , ::PyObject *aso_ref)
        : aop_root(acto, "accept") 
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

    virtual ~aop_accept() 
    {
        Py_XDECREF(lso_ref_);
        Py_XDECREF(aso_ref_);
    }

    virtual ::PyObject* dump(BOOL success, DWORD bytes_transferred);

    static const unsigned int addr_size = sizeof(sockaddr_in) + sizeof(sockaddr);
    static const unsigned int addr_buf_size = 2 * addr_size;

    unsigned char addr_buf_[addr_buf_size];

private:
    SOCKET lfd_, afd_;
    int lproto_, aproto_;
    ::PyObject *lso_ref_;
    ::PyObject *aso_ref_;
};


class aop_connect : public aop_root
{
public:
    aop_connect(::PyObject *acto, ::PyObject *so_ref, ::PyObject *addro)
        : aop_root(acto, "connect")
        , so_ref_(so_ref)
        , addro_(addro)
    {
        Py_XINCREF(so_ref);
        Py_XINCREF(addro);
    }

    virtual ~aop_connect()
    {
        Py_XDECREF(so_ref_);
        Py_XDECREF(addro_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);
private:
    ::PyObject *so_ref_;
    ::PyObject *addro_;
};

class aop_recv : public aop_root
{
public:
    aop_recv(::PyObject *acto, ::PyObject *so_ref
        , unsigned long size
        , unsigned long flags)
        : aop_root(acto, "recv")
        , so_ref_(so_ref)
    {
        buf_ = reinterpret_cast<char*>(malloc(size));
        size_ = size;
        flags_ = flags;
        Py_XINCREF(so_ref);
    }

    virtual ~aop_recv()
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

class aop_recvfrom : public aop_recv
{
public:
    aop_recvfrom(::PyObject *acto, ::PyObject *so_ref
    , ::PySocketSockObject *so
    , unsigned long size
    , unsigned long flags)
        : aop_recv(acto, so_ref, size, flags)
        , fromlen_(sizeof(from_))
        , fd_(so->sock_fd)
        , proto_(so->sock_proto)
    {
		name_ = "recvfrom";
    }

    virtual ~aop_recvfrom() {}

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);

    sockaddr * from() { return &from_; }
    int * fromlen() { return &fromlen_; }
private:
    SOCKET fd_;
    int proto_;
    sockaddr from_;
    int fromlen_;
};

class aop_send : public aop_root
{
public:
    aop_send(::PyObject *acto, ::PyObject *so_ref
        , unsigned long flags
        , ::PyObject *datao)
        : aop_root(acto, "send")
        , so_ref_(so_ref)
        , datao_(datao)
        , flags_(flags)
    {
        Py_XINCREF(so_ref);
        Py_XINCREF(datao);
    }

    virtual ~aop_send()
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

class aop_sendto : public aop_send
{
public:
    aop_sendto(::PyObject *acto, ::PyObject *so_ref
        , ::PyObject *addro
        , unsigned long flags
        , ::PyObject *datao)
        : aop_send(acto, so_ref, flags, datao)
        , addro_(addro)
    {
		name_ = "sendto";
        Py_XINCREF(addro);
    }

    virtual ~aop_sendto()
    {
        Py_XDECREF(addro_);
    }

    virtual ::PyObject * dump(BOOL success, DWORD bytes_transferred);

private:
    ::PyObject * addro_;
};

class aop_read : public aop_root
{
public:
    aop_read(::PyObject * acto, ::PyFileObject *fo
        , unsigned long long off
        , unsigned long size)
        : aop_root(acto, "read")
    {
        Offset = static_cast<DWORD>(off);
        OffsetHigh = static_cast<DWORD>((off >> (sizeof(DWORD) * 8)));
        size_ = size;
        buf_ = reinterpret_cast<char*>(malloc(size));
        Py_XINCREF(fo);
        fo_ = fo;
    }

    virtual ~aop_read()
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

class aop_write : public aop_root
{
public:
    aop_write(::PyObject *acto, ::PyFileObject *fo
        , unsigned long long off
        , ::PyObject *datao)
        : aop_root(acto, "write")
    {
        Offset = static_cast<DWORD>(off);
        OffsetHigh = static_cast<DWORD>((off >> (sizeof(DWORD) * 8)));
        Py_XINCREF(fo);
        fo_ = fo;
        Py_XINCREF(datao);
        datao_ = datao;
    }

    virtual ~aop_write()
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

#endif //  PYASYNCHIO_AOP_HPP_INCLUDED_