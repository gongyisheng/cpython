#include <Python.h>
#include <string>
#include <queue>
#include <iostream>

// Trie node struct
typedef struct TrieNode {
    bool isEnd;
    TrieNode *children[128];
} TrieNode;

// create
static TrieNode *create(bool _isEnd=false) {
    TrieNode *node = new TrieNode();
    node->isEnd = _isEnd;
    memset(node->children, NULL, 128);
    return node;
}

// insert a word to trie tree
static void insert(TrieNode* root, const std::string &text) {
    TrieNode* curr = root;
    for (int i=0;i<text.size();i++){
        int c = (int)text[i];
        TrieNode **curr_children = curr->children;
        if (curr_children[c] == NULL) {
            curr_children[c] = create();
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

// visualize
static void visualize(TrieNode* node, int level=0, char c='/') {
    for(int i=0;i<level*4;i++) {
        std::cout << " ";
    }
    std::cout << "Node [character=" << c << ",isEnd=" << node->isEnd << "]\n";
    for(int i=0;i<128;i++) {
        if(node->children[i] != NULL) {
            visualize(node->children[i], level+1, (char)i);
        }
    }
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

// free the tree from memory after usage
static void freeTree(TrieNode* node, bool is_root=true) {
    for(int i=0;i<128;i++) {
        if(node->children[i] != NULL) {
            freeTree(node->children[i], false);
            node->children[i] = NULL;
        }
    }
    if(!is_root) { // skip root, because root is attached with a python object
        delete node;
        node = NULL; // avoid dangling pointer
    }
}

/* Destructor function for points */
static void del_TrieNode(PyObject *obj) {
    freeTree((TrieNode*)PyCapsule_GetPointer(obj, "TrieNode"));
    free(PyCapsule_GetPointer(obj, "TrieNode"));
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
    TrieNode *node = create();
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

/* Visualize Radix Tree for Debug */
static PyObject *py_visualize(PyObject *self, PyObject *args) {
    TrieNode *root;
    PyObject *py_root;
    
    if (!PyArg_ParseTuple(args, "O", &py_root)) {
        return NULL;
    }
    if (!(root = PyTrie_AsTrieNode(py_root))) {
        return NULL;
    }
    visualize(root, 0);
    Py_RETURN_NONE;
}

/* Free the Tree from Memory after Usage */
static PyObject *py_free(PyObject *self, PyObject *args) {
    TrieNode *root;
    PyObject *py_root;
    
    if (!PyArg_ParseTuple(args, "O", &py_root)) {
        return NULL;
    }
    if (!(root = PyTrie_AsTrieNode(py_root))) {
        return NULL;
    }
    freeTree(root);
    Py_RETURN_NONE;
}

/* Module method table */
static PyMethodDef TrieMethods[] = {
    {"create", py_create, METH_VARARGS, "Create Trie Tree"},
    {"insert", py_insert, METH_VARARGS, "Insert word into Trie Tree"},
    {"isPartOf", py_isPartOf, METH_VARARGS, "Check if part of text exists in Trie Tree"},
    {"getMemoryUsage", py_getMemoryUsage, METH_VARARGS, "Get memory usage of Trie Tree"},
    {"visualize", py_visualize, METH_VARARGS, "Visualize Trie Tree for debug"},
    {"free", py_free, METH_VARARGS, "Free the Tree from Memory after Usage"},
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