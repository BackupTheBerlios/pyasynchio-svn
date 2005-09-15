#ifndef PYASYNCHIO_AIORESULT_HPP_INCLUDED_
#define PYASYNCHIO_AIORESULT_HPP_INCLUDED_

#pragma once
#include <pyasynchio/config.hpp>

#include <python.h>
#include <new>

namespace pyasynchio {

extern ::PyTypeObject aioresult_Type;

struct aioresult : PyObject {
    aioresult() 
    {
        dict_ = PyDict_New();
    }
    ~aioresult() 
    {
        Py_DECREF(dict_);
    }

    static ::PyObject * create()
    {
        PyObject * pr = aioresult_Type.tp_alloc(&aioresult_Type, 1);
        new (pr) aioresult();
        return pr;
    }

    static void aioresult::dealloc(aioresult *self)
    {
        self->~aioresult();
        self->ob_type->tp_free(self);
    }
    ::PyObject * dict_;
};

} // namespace pyasynchio

#endif // PYASYNCHIO_AIORESULT_HPP_INCLUDED_