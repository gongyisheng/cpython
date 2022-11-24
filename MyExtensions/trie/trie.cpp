#include <Python.h>
#include <unordered_map>
#include <string>
#include <queue>


// Trie node struct
typedef struct TrieNode {
    bool isEnd;
    std::unordered_map<char, TrieNode*> children;
} TrieNode;

static void insert(TrieNode* root, const std::string &text) {
    TrieNode* curr = root;
    for (int i=0;i<text.size();i++){
        char c = text[i];
        std::unordered_map<char, TrieNode*>& curr_children = curr->children;
        if (curr_children.find(c) == curr_children.end()) {
            curr_children[c] = new TrieNode();
        }
        curr = curr_children[c];
    }
    curr->isEnd = true;
}


static int isPartOf(TrieNode* root, const std::string &text){
    std::queue<TrieNode*> q;
    std::unordered_map<char, TrieNode*>& root_children = root->children;
    for (int i=0;i<text.size();i++) {
        char c = text[i];
        if(q.size()!=0){
            int size = q.size();
            for(int j=0;j<size;j++){
                TrieNode* node = q.front();
                std::unordered_map<char, TrieNode*>& node_children = node->children;
                q.pop();
                if(node_children.find(c)!=node_children.end()){
                    if(node_children[c]->isEnd){
                        return 1;
                    }
                    q.push(node_children[c]);
                }
            }
        }
        if(root_children.find(c)!=root_children.end()){
            q.push(root_children[c]);
        }
    }
    return 0;
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
    Py_Initialize();
    return PyModule_Create(&triemodule);
}