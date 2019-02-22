#define PYTHON2
/*
 *	Python module to reset the darhan log file
 *	Written by Yutaka Ishikawa, yutaka.ishikawa@riken.jp
 *	10/12/2018 Created
 */
#include "Python.h"
#include <stdio.h>

/*
 * darshan_reset will be hijacked by the darshan preload module.
 * If not, the following function is invoked to tell the user.
 */
extern void
darshan_single_reset(char *procname)
{
    fprintf(stderr, "%s: darshan_reset is invoked without darshan runtime\n",
	    procname);
}

static char procname[512];

static PyObject *
mydarshan_reset(PyObject *self, PyObject *args)
{
    const char	*cp;

    if (!PyArg_ParseTuple(args, "s", &cp)) {
	fprintf(stderr, "darshan_reset: String argument is required\n");
	Py_INCREF(Py_None);
	return Py_None;
    }
    fprintf(stderr, "cp =%s\n", cp);
    strncpy(procname, cp, 512);
    darshan_single_reset(procname);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef DarshanMethods[] = {
    {"reset",  mydarshan_reset, METH_VARARGS,
     "Rest darshan"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


#if defined PYTHON2
PyMODINIT_FUNC
initdarshan(void)
{
    (void) Py_InitModule("darshan", DarshanMethods);
}
#elif defined PYTHON3
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
#else
#error "Must define PYTHON2 or PYTHON3
#endif /* PYTHON3 */

