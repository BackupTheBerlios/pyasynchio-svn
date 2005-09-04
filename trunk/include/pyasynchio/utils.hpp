#ifndef PYASYNCHIO_UTILS_HPP_INCLUDED_
#define PYASYNCHIO_UTILS_HPP_INCLUDED_

#pragma once

#include <python.h>
#include "socketmodule.h"


namespace pyasynchio {

template<typename CPP_TYPE, typename PYTHON_TYPE_OBJ_TYPE>
CPP_TYPE* py_convert(PyObject *what, PYTHON_TYPE_OBJ_TYPE *type_obj)
{
    if (!PyObject_IsInstance(what, reinterpret_cast<PyObject*>(type_obj))) {
        PyErr_Format(PyExc_TypeError, "not a %s object", type_obj->tp_name);
        return NULL;
    }
    return reinterpret_cast<CPP_TYPE*>(what);
}

extern PySocketModule_APIObject socketmodule_api;


} // namespace pyasynchio

#endif // PYASYNCHIO_UTILS_HPP_INCLUDED_