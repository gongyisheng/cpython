#include <Python.h>
#include <string>

// get next array
static int *getNext(const std::string &pattern) {
    int i = 0, j = -1;
    int *next = new int[pattern.size() + 1];
    next[0] = -1;
    while (i < pattern.size()) {
        if (j == -1 || pattern[i] == pattern[j]) {
            i++;
            j++;
            next[i] = j;
        } else {
            j = next[j];
        }
    }
    return next;
}

// match text by pattern
static int match(const std::string &text, const std::string &pattern) {
    int *next = getNext(pattern);
    int i = 0, j = 0;
    while (i < text.size() && j < pattern.size()) {
        if (j == -1 || text[i] == pattern[j]) {
            i++;
            j++;
        } else {
            j = next[j];
        }
    }
    delete[] next;
    if(j == pattern.size()){
        return i-j;
    }
    return -1;
}

/* Python Wrapper Functions*/
/* Destructor function for points */
static void del_kmp(PyObject *obj) {
    free(PyCapsule_GetPointer(obj, NULL));
}

// match
static PyObject *py_match(PyObject *self, PyObject *args) {
    const char *text;
    const char *pattern;
    if (!PyArg_ParseTuple(args, "OO", &text, &pattern)) {
        return NULL;
    }
    int result = match(text, pattern);
    return Py_BuildValue("i", result);
}

/* Module method table */
static PyMethodDef kmpMethods[] = {
    {"match", py_match, METH_VARARGS, "Match text with pattern"},
    {NULL, NULL, 0, NULL}
};

/* Module definition structure */
static struct PyModuleDef kmpmodule = {
    PyModuleDef_HEAD_INIT,  /* m_base */
    "kmp",                  /* name of module */
    "kmp",                  /* module documentation, may be NULL */
    -1,                     /* size of per-interpreter state or -1 */
    kmpMethods              /* method table */
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_kmp(void) {
    Py_Initialize();
    return PyModule_Create(&kmpmodule);
};