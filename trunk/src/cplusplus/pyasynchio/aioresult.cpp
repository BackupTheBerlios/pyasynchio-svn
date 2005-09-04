#include <pyasynchio/aioresult.hpp>

namespace pyasynchio {

PyTypeObject aioresult_Type = {
    PyObject_HEAD_INIT(NULL)
    0,                                  // ob_size
    "_pyasynchio.aioresult",            // tp_name
    sizeof(aioresult),                  // tp_basicsize
    0,                                  // tp_itemsize
    (destructor)&aioresult::dealloc,    // tp_dealloc
    0,                                  // tp_print
    0,                                  // tp_getattr
    0,                                  // tp_setattr
    0,                                  // tp_compare
    0,                                  // tp_repr
    0,                                  // tp_as_number
    0,                                  // tp_as_sequence
    0,                                  // tp_as_mapping
    0,                                  // tp_hash 
    0,                                  // tp_call
    0,                                  // tp_str
    &PyObject_GenericGetAttr,           // tp_getattro
    &PyObject_GenericSetAttr,           // tp_setattro
    0,                                  // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                 // tp_flags
    "aioresult objects",                // tp_doc 

    0,                                  // tp_traverse 
    0,                                  // tp_clear
    0,                                  // tp_richcompare 
    0,                                  // tp_weaklistoffset 

    0,                                  // tp_iter
    0,                                  // tp_iternext 

    0,                                  // tp_methods 
    0,                                  // tp_members
    0,                                  // tp_getset
    0,                                  // tp_base 
    
    0,                                  // tp_dict
    0,                                  // tp_descr_get
    0,                                  // tp_descr_set
    offsetof(aioresult, dict_),         // tp_dictoffset 
    0,                                  // tp_init
    0,                                  // tp_alloc 
    &PyType_GenericNew,                 // tp_new
    0,                                  // tp_free
    0,                                  // tp_is_gc
    0,                                  // tp_bases
    0,                                  // tp_mro
    0,                                  // tp_cache
    0,                                  // tp_subclasses
    0,                                  // tp_weaklist
    0,                                  // tp_del
};


} // namespace pyasynchio