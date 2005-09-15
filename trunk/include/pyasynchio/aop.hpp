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

#include <pyasynchio/config.hpp>
#include <pyasynchio/aioresult.hpp>
#include "socketmodule.h"

namespace pyasynchio {

class aop_root : public platform::aop_impl<aop_root>
{
protected:
    ::PyObject *acto_;
	const char * name_;

public:
    aop_root(::PyObject *acto, const char * name) 
        : acto_(acto)
		, name_(name)
		, platform::aop_impl<aop_root>()
    {
        Py_XINCREF(acto);
    }
    virtual ~aop_root() 
    {
        Py_XDECREF(acto_);
    }

	virtual void init()
	{
		platform::aop_impl<aop_root>::init();
	}

    virtual ::PyObject* dump(bool success, size_t bytes_count) = 0;

	::PyObject * to_python(bool success, size_t bytes_count)
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

		::PyObject * speco = this->dump(success, bytes_count);
		::PyObject * tp = Py_BuildValue("(OO)", geno, speco);
		Py_DECREF(geno);
		Py_DECREF(speco);
		return tp;
	}
};

class aop_accept : public aop_root, platform::aop_impl<aop_accept>
{
public:
    aop_accept(::PyObject *acto
        , ::PySocketSockObject *lsocko
        , ::PySocketSockObject *asocko
        , ::PyObject *lsock_refo
        , ::PyObject *asock_refo)
        : aop_root(acto, "accept") 
		, lsocko_(lsocko)
		, asocko_(asocko)
        , lsock_refo_(lsock_refo)
        , asock_refo_(asock_refo)
		, platform::aop_impl<aop_accept>()
    {
		Py_XINCREF(lsocko_);
		Py_XINCREF(asocko_);
        Py_XINCREF(lsock_refo_);
        Py_XINCREF(asock_refo_);
    }

	virtual void init()
	{
		aop_root::init();
		platform::aop_impl<aop_accept>::init();
	}

    virtual ~aop_accept() 
    {
        Py_XDECREF(lsock_refo_);
        Py_XDECREF(asock_refo_);
		Py_XDECREF(lsocko_);
		Py_XDECREF(asocko_);
    }

    virtual ::PyObject* dump(bool success, size_t bytes_count);

    static const unsigned int addr_size = sizeof(sockaddr_in) + sizeof(sockaddr);
    static const unsigned int addr_buf_size = 2 * addr_size;

    unsigned char addr_buf_[addr_buf_size];
	::PySocketSockObject * lsocko() const { return lsocko_; }
	::PySocketSockObject * asocko() const { return asocko_; }

private:
    ::PyObject *lsock_refo_, *asock_refo_;
	::PySocketSockObject *lsocko_, *asocko_;
};

class aop_connect : public aop_root, public platform::aop_impl<aop_connect>
{
public:
    aop_connect(::PyObject *acto
		, ::PySocketSockObject *socko
		, ::PyObject *sock_refo
		, ::PyObject *addro)
        : aop_root(acto, "connect")
		, socko_(socko)
        , sock_refo_(sock_refo)
        , addro_(addro)
		, platform::aop_impl<aop_connect>()
    {
        Py_XINCREF(sock_refo);
        Py_XINCREF(addro);
		Py_XINCREF(socko);
    }

	virtual void init()
	{
		aop_root::init();
		platform::aop_impl<aop_connect>::init();
	}


    virtual ~aop_connect()
    {
        Py_XDECREF(sock_refo_);
        Py_XDECREF(addro_);
		Py_XDECREF(socko_);
    }

    virtual ::PyObject * dump(bool success, size_t bytes_count);
	::PySocketSockObject * socko() const { return socko_; }
	::PyObject * addro() const { return addro_; }
private:
    ::PyObject *sock_refo_;
    ::PyObject *addro_;
	::PySocketSockObject *socko_;
	::sockaddr addr_;
	int addrlen_;
};

class aop_recv : public aop_root, public platform::aop_impl<aop_recv>
{
public:
    aop_recv(::PyObject *acto
		, ::PySocketSockObject *socko
		, ::PyObject *sock_refo
        , unsigned long bufsize
        , unsigned long flags)
        : aop_root(acto, "recv")
		, socko_(socko)
        , sock_refo_(sock_refo)
		, platform::aop_impl<aop_recv>()
    {
        buf_ = reinterpret_cast<char*>(malloc(bufsize));
        bufsize_ = bufsize;
        flags_ = flags;
		Py_XINCREF(socko_);
        Py_XINCREF(sock_refo_);
    }

	virtual void init()
	{
		aop_root::init();
		platform::aop_impl<aop_recv>::init();
	}

    virtual ~aop_recv()
    {
        Py_XDECREF(sock_refo_);
		Py_XDECREF(socko_);
        free(buf_);
    }

    virtual ::PyObject *dump(bool success, size_t bytes_count);


    char * buf() const { return buf_; }
	unsigned long bufsize() const { return bufsize_; }
	unsigned long flags() const { return flags_; }
	::PySocketSockObject * socko() { return socko_; }

protected:
	::PySocketSockObject *socko_;
    ::PyObject *sock_refo_;
private:
    unsigned long bufsize_, flags_;
    char * buf_;
};

class aop_recvfrom : public aop_recv, public platform::aop_impl<aop_recvfrom>
{
public:
    aop_recvfrom(::PyObject *acto
		, ::PySocketSockObject *socko
		, ::PyObject *sock_refo
		, unsigned long size
		, unsigned long flags)
        : aop_recv(acto, socko, sock_refo, size, flags)
        , fromlen_(sizeof(from_))
		, platform::aop_impl<aop_recvfrom>()
    {
		name_ = "recvfrom";
    }

	virtual void init()
	{
		aop_recv::init();
		platform::aop_impl<aop_recvfrom>::init();
	}

    virtual ~aop_recvfrom() {}

    virtual ::PyObject * dump(bool success, size_t bytes_count);

    sockaddr * from() { return &from_; }
    int * fromlen() { return &fromlen_; }
private:
    sockaddr from_;
    int fromlen_;
};

class aop_send : public aop_root, public platform::aop_impl<aop_send>
{
public:
    aop_send(::PyObject *acto
		, ::PySocketSockObject *socko
		, ::PyObject *sock_refo
        , unsigned long flags
        , ::PyObject *datao)
        : aop_root(acto, "send")
		, socko_(socko)
        , sock_refo_(sock_refo)
        , datao_(datao)
        , flags_(flags)
		, platform::aop_impl<aop_send>()
    {
		Py_XINCREF(socko_);
        Py_XINCREF(sock_refo_);
        Py_XINCREF(datao_);
    }

    virtual ~aop_send()
    {
		Py_XDECREF(socko_);
        Py_XDECREF(sock_refo_);
        Py_XDECREF(datao_);
    }

	virtual void init()
	{
		aop_root::init();
		platform::aop_impl<aop_send>::init();
	}


    virtual ::PyObject * dump(bool success, size_t bytes_count);
	::PySocketSockObject * socko() { return socko_; }
	::PyObject * datao() { return datao_; }
	unsigned long flags() const { return flags_; }
private:
	::PySocketSockObject *socko_;
    ::PyObject *sock_refo_;
    ::PyObject *datao_;
    unsigned long flags_;
};

class aop_sendto : public aop_send, public platform::aop_impl<aop_sendto>
{
public:
    aop_sendto(::PyObject *acto
		, ::PySocketSockObject *socko
		, ::PyObject *sock_refo
        , ::PyObject *addro
        , unsigned long flags
        , ::PyObject *datao)
        : aop_send(acto, socko, sock_refo, flags, datao)
        , addro_(addro)
		, platform::aop_impl<aop_sendto>()
    {
		name_ = "sendto";
        Py_XINCREF(addro);
    }

    virtual ~aop_sendto()
    {
        Py_XDECREF(addro_);
    }

	virtual void init()
	{
		aop_send::init();
		platform::aop_impl<aop_sendto>::init();
	}


    virtual ::PyObject * dump(bool success, size_t bytes_count);
	::PyObject * addro() { return addro_; }
private:
    ::PyObject * addro_;
};

class aop_read : public aop_root, public platform::aop_impl<aop_read>
{
public:
    aop_read(::PyObject * acto, ::PyFileObject *fo
        , unsigned long long offs
        , unsigned long size)
        : aop_root(acto, "read")
		, size_(size)
		, off_(offs)
		, buf_(reinterpret_cast<char*>(malloc(size)))
		, fo_(fo)
		, platform::aop_impl<aop_read>()
    {
        Py_XINCREF(fo);
    }

    virtual ~aop_read()
    {
        free(buf_);
        Py_XDECREF(fo_);
    }

	virtual void init()
	{
		aop_root::init();
		platform::aop_impl<aop_read>::init();
	}


    virtual ::PyObject * dump(bool success, size_t bytes_transferred);
    char * buf() const { return buf_; }
	unsigned long size() const { return size_; }
	unsigned long long off() const { return off_; }
	::PyFileObject * fileo() { return fo_; }

private:
    ::PyFileObject * fo_;
    char * buf_;
    unsigned long size_;
	unsigned long long off_;
};

class aop_write : public aop_root, public platform::aop_impl<aop_write>
{
public:
    aop_write(::PyObject *acto, ::PyFileObject *fo
        , unsigned long long offs
        , ::PyObject *datao)
        : aop_root(acto, "write")
		, fo_(fo)
		, datao_(datao)
		, off_(offs)
		, platform::aop_impl<aop_write>()
    {
        Py_XINCREF(fo);
        Py_XINCREF(datao);
    }

    virtual ~aop_write()
    {
        Py_XDECREF(fo_);
        Py_XDECREF(datao_);
    }

	virtual void init()
	{
		aop_root::init();
		platform::aop_impl<aop_write>::init();
	}


    virtual ::PyObject * dump(bool success, size_t bytes_count);
	unsigned long long off() const { return off_; }
	::PyFileObject * fileo() { return fo_; }
	::PyObject * datao() { return datao_; }
private:
    ::PyFileObject * fo_;
    ::PyObject * datao_;
	unsigned long long off_;
};




}

#endif //  PYASYNCHIO_AOP_HPP_INCLUDED_