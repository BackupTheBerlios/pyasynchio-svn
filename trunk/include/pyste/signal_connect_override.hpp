#pragma once
#include <boost/signal.hpp>
#include <boost/python.hpp>

template<class T>
boost::python::object to_object(T const& x)
{
    return boost::python::object(x);
}

template<>
boost::python::object to_object<void*>(void* const &x)
{
    return boost::python::object(
        boost::python::handle<>(PyCObject_FromVoidPtr(x, 0)));
}

template<>
boost::python::object to_object<const void*>(const void* const &x)
{
    return to_object<void*>(const_cast<void*>(x));
}

class obj_wrapper
{
public:
    obj_wrapper(boost::python::object obj)
    :_obj(obj)
    {}

    boost::python::object operator()()
    {
        return _obj();
    }

    template<typename A0>
    boost::python::object operator()(A0 const &arg0)
    {
        return _obj(to_object<A0>(arg0));
    }

    template<typename A0
        , typename A1>
    boost::python::object operator()(A0 const &arg0
        , A1 const &arg1)
    {
        return _obj(to_object<A0>(arg0)
            , to_object<A1>(arg1));
    }

    template<typename A0
        , typename A1
        , typename A2>
    boost::python::object operator()(A0 const &arg0
        , A1 const &arg1
        , A2 const &arg2)
    {
        return _obj(to_object<A0>(arg0)
            , to_object<A1>(arg1)
            , to_object<A2>(arg2));
    }

    template<typename A0
        , typename A1
        , typename A2
        , typename A3>
    boost::python::object operator()(A0 const &arg0
        , A1 const &arg1
        , A2 const &arg2
        , A3 const &arg3)
    {
        return _obj(to_object<A0>(arg0)
            , to_object<A1>(arg1)
            , to_object<A2>(arg2)
            , to_object<A3>(arg3));
    }


    template<typename A0
        , typename A1
        , typename A2
        , typename A3
        , typename A4>
    boost::python::object operator()(A0 const &arg0
        , A1 const &arg1
        , A2 const &arg2
        , A3 const &arg3
        , A4 const &arg4)
    {
        return _obj(to_object<A0>(arg0)
            , to_object<A1>(arg1)
            , to_object<A2>(arg2)
            , to_object<A3>(arg3)
            , to_object<A4>(arg4));
    }


    template<typename A0
        , typename A1
        , typename A2
        , typename A3
        , typename A4
        , typename A5>
    boost::python::object operator()(A0 const &arg0
        , A1 const &arg1
        , A2 const &arg2
        , A3 const &arg3
        , A4 const &arg4
        , A5 const &arg5)
    {
        return _obj(to_object<A0>(arg0)
            , to_object<A1>(arg1)
            , to_object<A2>(arg2)
            , to_object<A3>(arg3)
            , to_object<A4>(arg4)
            , to_object<A5>(arg5));
    }
private:
    boost::python::object _obj;
};

template<class SIG>
boost::signals::connection connect_override_one(SIG *sig
    , boost::python::object o)
{
    return sig->connect(obj_wrapper(o));
}


template<class SIG>
boost::signals::connection connect_override_two(SIG *sig
    , boost::python::object o
    , boost::signals::connect_position at = boost::signals::at_back)
{
    return sig->connect(obj_wrapper(o), at);
}

template<class SIG>
boost::signals::connection connect_override_three(SIG *sig
    , const typename SIG::group_type &group
    , const boost::python::object o)
{
    return sig->connect(group, obj_wrapper(o));
}


template<class SIG>
boost::signals::connection connect_override_four(SIG *sig
    , const typename SIG::group_type &group
    , const boost::python::object o
    , boost::signals::connect_position at = boost::signals::at_back)
{
    return sig->connect(group, obj_wrapper(o), at);
}
