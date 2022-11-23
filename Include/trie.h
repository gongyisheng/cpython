#include <Python.h>
#include <unordered_map>
#include <string>
#include <queue>


// Trie node struct
typedef struct TrieNode {
    bool isEnd;
    std::unordered_map<char, TrieNode*> children;
} TrieNode;

static void insert(TrieNode* root, const std::string* text) {
    TrieNode* curr = root;
    char c;
    for (int i=0;i<(*text).size();i++){
        c = (*text)[i];
        if (curr->children.find(c) == curr->children.end()) {
            curr->children[c] = (TrieNode *) malloc(sizeof(TrieNode));
        }
        curr = curr->children[c];
    }
    curr->isEnd = true;
}

static short int isPartOf(TrieNode* root, const std::string* text){
    std::queue<TrieNode*> q;
    char c;
    for (int i=0;i<(*text).size();i++) {
        if(q.size()!=0){
            int size = q.size();
            for(int j=0;j<size;j++){
                TrieNode* node = q.front();
                c = (*text)[i];
                q.pop();
                if(node->children.find(c)!=node->children.end()){
                    if(node->children[c]->isEnd){
                        return true;
                    }
                    q.push(node->children[c]);
                }
            }
        }
        if(root->children.find(c)!=root->children.end()){
            q.push(root->children[c]);
        }
    }
    return false;
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
    TrieNode *node = (TrieNode *) malloc(sizeof(TrieNode));
    node->isEnd = false;
    return PyTrie_FromTrieNode(node, 1);
}

/* Insert word into Trie Tree */
static PyObject *py_insert(PyObject *self, PyObject *args) {
    TrieNode *root;
    PyObject *py_root;
    const std::string *text;
    
    if (!PyArg_ParseTuple(args, "OO", &py_root, &text)) {
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
    const std::string *text;
    
    if (!PyArg_ParseTuple(args, "OO", &py_root, &text)) {
        return NULL;
    }
    if (!(root = PyTrie_AsTrieNode(py_root))) {
        return NULL;
    }
    short int result = isPartOf(root, text);
    Py_BuildValue("h", result);
}

/* Module method table */
static PyMethodDef TrieMethods[] = {
    {"create", py_create, METH_VARARGS, "Create Trie Tree"},
    {"insert", py_insert, METH_VARARGS, "Insert word into Trie Tree"},
    {"isPartOf", py_isPartOf, METH_VARARGS, "Check if part of text exists in Trie Tree"},
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
    return PyModule_Create(&triemodule);
}