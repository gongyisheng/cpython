#include <Python.h>
#include <string>

struct kmp_next {
    int *next;
};

static kmp_next *createNext(int *next){
    kmp_next *kmp = new kmp_next;
    kmp->next = next;
    return kmp;
}

// get next array
static kmp_next *getNext(const std::string &pattern) {
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
    return createNext(next);
}

// match text by pattern
static int match(const std::string &text, const std::string &pattern) {
    kmp_next *kmpNext = getNext(pattern);
    int *next = kmpNext->next;
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

// match with cached next
static int matchWithNext(const std::string &text, const std::string &pattern, kmp_next *kmpNext) {
    int *next = kmpNext->next;
    int i = 0, j = 0;
    while (i < text.size() && j < pattern.size()) {
        if (j == -1 || text[i] == pattern[j]) {
            i++;
            j++;
        } else {
            j = next[j];
        }
    }
    if(j == pattern.size()){
        return i-j;
    }
    return -1;
}

/* Python Wrapper Functions*/
/* Destructor function for points */
static void del_kmpnext(PyObject *obj) {
    free(PyCapsule_GetPointer(obj, "kmp_next"));
}

/* Utility functions */
static kmp_next *Pykmp_Askmpnext(PyObject *obj) {
    return (kmp_next *) PyCapsule_GetPointer(obj, "kmp_next");
}

static PyObject *Pykmp_Fromkmpnext(kmp_next *next, int must_free) {
    return PyCapsule_New(next, "kmp_next", must_free ? del_kmpnext : NULL);
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

// get cached next
static PyObject *py_getNext(PyObject *self, PyObject *args) {
    const char *pattern;
    if (!PyArg_ParseTuple(args, "O", &pattern)) {
        return NULL;
    }
    kmp_next *next = getNext(pattern);
    return Pykmp_Fromkmpnext(next, 1);
}

// match with cached next
static PyObject *py_matchWithNext(PyObject *self, PyObject *args) {
    const char *text;
    const char *pattern;
    PyObject *py_next;
    kmp_next *next;
    if (!PyArg_ParseTuple(args, "OOO", &text, &pattern, &py_next)) {
        return NULL;
    }
    if (!(next = Pykmp_Askmpnext(py_next))) {
        return NULL;
    }
    int result = matchWithNext(text, pattern, next);
    return Py_BuildValue("i", result);
}

/* Module method table */
static PyMethodDef kmpMethods[] = {
    {"getNext", py_getNext, METH_VARARGS, "Get cached next array"},
    {"match", py_match, METH_VARARGS, "Match text with pattern"},
    {"matchWithNext", py_matchWithNext, METH_VARARGS, "Match with cached next"},
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