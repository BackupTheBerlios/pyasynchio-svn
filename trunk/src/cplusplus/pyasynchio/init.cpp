#include <pyasynchio/apoll.hpp>
#include <pyasynchio/aioresult.hpp>
#include <pyasynchio/utils.hpp>

namespace pyasynchio {

::PyObject * get_sock_family(::PyObject *self, ::PyObject *args)
{
    ::PyObject *so_raw;
    if(!PyArg_ParseTuple(args, "O:get_sock_family", &so_raw)) {
        return NULL;
    }
    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    return PyInt_FromLong(so->sock_family);
}

::PyObject * get_sock_type(::PyObject *self, ::PyObject *args)
{
    ::PyObject *so_raw;
    if(!PyArg_ParseTuple(args, "O:get_sock_type", &so_raw)) {
        return NULL;
    }
    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    return PyInt_FromLong(so->sock_type);
}

::PyObject * get_sock_proto(::PyObject *self, ::PyObject *args)
{
    ::PyObject *so_raw;
    if(!PyArg_ParseTuple(args, "O:get_sock_proto", &so_raw)) {
        return NULL;
    }
    PySocketSockObject *so = py_convert<PySocketSockObject>(so_raw
        , PySocketModule.Sock_Type);
    if (NULL == so) {
        return NULL;
    }

    return PyInt_FromLong(so->sock_proto);
}

static PyMethodDef pyasynchio_methods[] = {
    { "get_sock_family"
        , reinterpret_cast<PyCFunction>(&get_sock_family)
        , METH_VARARGS
        , "get socket family constant"
    }
    , { "get_sock_type"
        , reinterpret_cast<PyCFunction>(&get_sock_type)
        , METH_VARARGS
        , "get socket type constant"
    }
    , { "get_sock_proto"
        , reinterpret_cast<PyCFunction>(&get_sock_proto)
        , METH_VARARGS
        , "get socket protocol constant"
    }
    , {NULL}  /* Sentinel */
};

} // namespace pyasynchio

void init_pyasynchio()
{
    using namespace pyasynchio;
    ::PySocketModule_ImportModuleAndAPI();
	socketmodule_api = PySocketModule;

    if (PyType_Ready(&apoll_Type) < 0) {
        return;
    }
    Py_INCREF(&apoll_Type);

    if (PyType_Ready(&aioresult_Type) < 0) {
        return;
    }
    Py_INCREF(&aioresult_Type);

    PyObject *m = Py_InitModule("_pyasynchio", pyasynchio_methods);

    PyModule_AddObject(m
        , "apoll"
        , reinterpret_cast<PyObject*>(&apoll_Type) );

    PyModule_AddObject(m
        , "aioresult"
        , reinterpret_cast<PyObject*>(&aioresult_Type) );

}


