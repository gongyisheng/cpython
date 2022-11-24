#include <Python.h>
#include <string>
#include <queue>


// Trie node struct
typedef struct TrieNode {
    bool isEnd;
    TrieNode *children[128];
} TrieNode;

// insert a word to trie tree
static void insert(TrieNode* root, const std::string &text) {
    TrieNode* curr = root;
    for (int i=0;i<text.size();i++){
        int c = (int)text[i];
        TrieNode **curr_children = curr->children;
        if (curr_children[c] == NULL) {
            curr_children[c] = new TrieNode();
        }
        curr = curr_children[c];
    }
    curr->isEnd = true;
}

// find whether any substring in text is in trie tree
static int isPartOf(TrieNode* root, const std::string &text){
    std::queue<TrieNode*> q;
    auto root_children = root->children;
    for (int i=0;i<text.size();i++) {
        char c = (int)text[i];
        if(q.size()!=0){
            int size = q.size();
            for(int j=0;j<size;j++){
                TrieNode *node = q.front();
                TrieNode **node_children = node->children;
                q.pop();
                if(node_children[c] != NULL){
                    if(node_children[c]->isEnd){
                        return 1;
                    }
                    q.push(node_children[c]);
                }
            }
        }
        if(root_children[c] != NULL){
            q.push(root_children[c]);
        }
    }
    return 0;
}

// calc memory usage of trie tree
static int getMemoryUsage(TrieNode* root) {
    int size = sizeof(root) + sizeof(root->isEnd) + sizeof(root->children);
    TrieNode **root_children = root->children;
    for(int i=0;i<128;i++) {
        if(root_children[i] != NULL) {
            size += getMemoryUsage(root_children[i]);
        }
    }
    return size;
}


/* Destructor function for points */
static void del_TrieNode(PyObject *obj) {
    free(PyCapsule_GetPointer(obj,"TrieNode"));
}

/* Utility functions */
static TrieNode *PyTrie_AsTrieNode(PyObject *obj) {
    return (TrieNode *) PyCapsule_GetPointer(obj, "TrieNode");
}

static PyObject *PyTrie_FromTrieNode(TrieNode *node, int must_free) {
    return PyCapsule_New(node, "TrieNode", must_free ? del_TrieNode : NULL);
}

/* Create Trie Tree */
static PyObject *py_create(PyObject *self, PyObject *args) {
    TrieNode *node = new TrieNode();
    node->isEnd = false;
    memset(node->children, NULL, 128);
    return PyTrie_FromTrieNode(node, 1);
}

/* Insert word into Trie Tree */
static PyObject *py_insert(PyObject *self, PyObject *args) {
    TrieNode *root;
    PyObject *py_root;
    const char *text;
    
    if (!PyArg_ParseTuple(args, "Os", &py_root, &text)) {
        return NULL;
    }
    if (!(root = PyTrie_AsTrieNode(py_root))) {
        return NULL;
    }
    insert(root, text);
    Py_RETURN_NONE;
}

/* Check if part of text exists in Trie Tree */
static PyObject *py_isPartOf(PyObject *self, PyObject *args) {
    TrieNode *root;
    PyObject *py_root;
    const char *text;
    
    if (!PyArg_ParseTuple(args, "Os", &py_root, &text)) {
        return NULL;
    }
    if (!(root = PyTrie_AsTrieNode(py_root))) {
        return NULL;
    }
    int result = isPartOf(root, text);
    return Py_BuildValue("i", result);
}

/* Get Memory Usage of Trie Tree */
static PyObject *py_getMemoryUsage(PyObject *self, PyObject *args) {
    TrieNode *root;
    PyObject *py_root;
    
    if (!PyArg_ParseTuple(args, "O", &py_root)) {
        return NULL;
    }
    if (!(root = PyTrie_AsTrieNode(py_root))) {
        return NULL;
    }
    int memSize = getMemoryUsage(root);
    return Py_BuildValue("i", memSize);
}

/* Module method table */
static PyMethodDef TrieMethods[] = {
    {"create", py_create, METH_VARARGS, "Create Trie Tree"},
    {"insert", py_insert, METH_VARARGS, "Insert word into Trie Tree"},
    {"isPartOf", py_isPartOf, METH_VARARGS, "Check if part of text exists in Trie Tree"},
    {"getMemoryUsage", py_getMemoryUsage, METH_VARARGS, "Get memory usage of Trie Tree"},
    {NULL, NULL, 0, NULL}
};

/* Module definition structure */
static struct PyModuleDef triemodule = {
    PyModuleDef_HEAD_INIT,  /* m_base */
    "trie",                 /* name of module */
    "Trie Tree",            /* module documentation, may be NULL */
    -1,                     /* size of per-interpreter state or -1 */
    TrieMethods             /* method table */
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_trie(void) {
    Py_Initialize();
    return PyModule_Create(&triemodule);
}