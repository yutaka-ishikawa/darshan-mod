#include "Python.h"

static PyObject *
darshan_reset(PyObject *self, PyObject *arg)
{
    printf("darshan_reset is invoked\n");
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef DarshanMethods[] = {
    {"reset",  darshan_reset, METH_VARARGS,
     "Rest darshan"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef darshanmodule = {
    PyModuleDef_HEAD_INIT,
    "darshan",   /* name of module */
    0, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    DarshanMethods
};

PyMODINIT_FUNC
PyInit_darshan(void)
{
    return PyModule_Create(&darshanmodule);
}
