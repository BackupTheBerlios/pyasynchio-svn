#ifndef PYSTE_BOOST_SHARED_PTR_CREATE_HPP_INCLUDED_
#define PYSTE_BOOST_SHARED_PTR_CREATE_HPP_INCLUDED_

#pragma once

#include <boost/shared_ptr.hpp>

template<typename T>
boost::shared_ptr<T> shared_ptr_create()
{
    return boost::shared_ptr<T>(new T());
}

template<typename T, typename P0>
boost::shared_ptr<T> shared_ptr_create(P0 _p0)
{
    return boost::shared_ptr<T>(new T(_p0));
}

template<typename T, typename P0, typename P1>
boost::shared_ptr<T> shared_ptr_create(P0 _p0, P1 _p1)
{
    return boost::shared_ptr<T>(new T(_p0, _p1));
}

#endif // PYSTE_BOOST_SHARED_PTR_CREATE_HPP_INCLUDED_