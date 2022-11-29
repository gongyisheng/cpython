#include <Python.h>
#include <string>
#include <iostream>

// Radix node is a kind of compressed Trie node
// Radix tree wiki: https://en.wikipedia.org/wiki/Radix_tree
// Visualization simulator: https://www.cs.usfca.edu/~galles/visualization/RadixTree.html

//debug mode 
//#define DEBUG_MODE

// Radix node struct
typedef struct RadixNode {
    bool isEnd;
    int height;
    std::string word;
    RadixNode *children[128];
} RadixNode;

// swap children between two radix nodes
// TODO: this func can be improved if node->children is a pointer to array. 
static void swapChildren(RadixNode *a, RadixNode *b) {
    for(int i=0;i<128;i++){
        RadixNode *tmp = a->children[i];
        a->children[i] = b->children[i];
        b->children[i] = tmp;
    }
}

// create a dynamic radix node, should be free from memory after usage
static RadixNode *create(bool _isEnd=false, std::string _word="") {
    RadixNode *node = new RadixNode();
    node->isEnd = _isEnd;
    node->word = _word;
    node->height = 0;
    memset(node->children, NULL, 128);
    return node;
}

// insert a word to radix tree
static void insert(RadixNode *root, const std::string &text) {
    root->height = std::max(root->height, (int)text.size());
    RadixNode* curr = root;
    int i = 0;
    while(i<text.size()) {
        int c = (int)text[i];
        RadixNode **curr_children = curr->children;
        // if the current node has no child with `c` as perfix
        if (curr_children[c] == NULL) {
            RadixNode *nnode = create(true, text.substr(i, text.size()-i));
            curr_children[c] = nnode;
            break;
        }

        // if the current node has a child with `c` as perfix
        std::string curr_word = curr_children[c]->word;
        // compare & see if the perfix of existing word and the new word are the same
        int j = 0;
        while(j<curr_word.size() && i<text.size() && curr_word[j]==text[i]) {
            j++;
            i++;
        }

        // if reach the end of the existing word
        if(j==curr_word.size()) {
            curr = curr_children[c];
        }
        else { // if not reach the end of the existing word
            // create a new node to be appended to the existing node (left node)
            RadixNode *pnode = curr_children[c];
            RadixNode *lnode = create(pnode->isEnd, curr_word.substr(j, curr_word.size()-j));
            swapChildren(lnode, pnode);
            // update the existing node (parent node)
            pnode->word = curr_word.substr(0, j);
            pnode->isEnd = false;
            pnode->children[(int)lnode->word[0]] = lnode;
            // if the new word is not longer than the existing word
            if(i==text.size()) {
                curr_children[c]->isEnd = true;
            } 
            else { // if the new word is longer than the existing word, create a new node (right node)
                RadixNode *rnode = create(true, text.substr(i));
                curr_children[c]->children[(int)rnode->word[0]] = rnode;
            }
            break;
        }
    }
}

// find whether the full given text is in radix tree
static int matchFull(RadixNode *root, const std::string &text){
    RadixNode *curr = root;
    int i = 0;
    while(i<text.size()){
        int c = (int)text[i];
        RadixNode **curr_children = curr->children;
        if(curr_children[c] == NULL){
            return 0;
        }
        std::string curr_word = curr_children[c]->word;
        int j = 0;
        while(j<curr_word.size() && i<text.size() && curr_word[j]==text[i]){
            j++;
            i++;
        }
        if(j==curr_word.size()){
            curr = curr_children[c];
        }
        else{
            return 0;
        }
    }
    return curr->isEnd;
}

// find whether any substring of the given text is in radix tree
static int matchSub(RadixNode *root, const std::string &text){
    RadixNode *node_queue[root->height];
    int index_queue[root->height];
    int q_tail = 0;
    RadixNode **root_children = root->children;
    for (int i=0;i<text.size();i++) {
        int c = (int)text[i];
        int new_tail = 0;
        for(int j=0;j<q_tail;j++){
            RadixNode *node = node_queue[j];
            RadixNode **node_children = node->children;
            int index = index_queue[j];
            // if cursor is at the end of the word
            if(index==node->word.size()) {
                // reach the end
                if(node->isEnd) {
                    return 1;
                }
                // has next node
                else if(node_children[c] != NULL) {
                    node_queue[new_tail] = node_children[c];
                    index_queue[new_tail] = 1;
                    new_tail++;
                }
            }
            else if(node->word[index]==text[i]) {
                node_queue[new_tail] = node;
                index_queue[new_tail] = index+1;
                new_tail++;
            }
        }
        if(root_children[c] != NULL){
            node_queue[new_tail] = root_children[c];
            index_queue[new_tail] = 1;
            new_tail++;
        }
        q_tail = new_tail;
        #ifdef DEBUG_MODE
        std::cout << "char: " << text[i] << ", q_tail: " << q_tail << ", queue:[";
        for(int j=0;j<q_tail;j++){
            std::cout << "(" <<q[j].first->word << "," << q[j].second << ") ";
        }
        std::cout << "]" << std::endl;
        #endif
    }

    // handle cases that the substring ends with last character matches
    for(int i=0;i<q_tail;i++){
        if(node_queue[i]->isEnd) {
            return 1;
        }
    }
    return 0;
}

// visualize
static void visualize(RadixNode *node, int level=0) {
    for(int i=0;i<level*4;i++) {
        std::cout << " ";
    }
    std::cout << "Node [address=" << node << ", word=" << node->word << ", isEnd=" << node->isEnd << "]\n";
    for(int i=0;i<128;i++) {
        if(node->children[i] != NULL) {
            visualize(node->children[i], level+1);
        }
    }
}

// calc memory usage of radix tree
static int getMemoryUsage(RadixNode *node) {
    int size = sizeof(node) + sizeof(node->word) + sizeof(node->isEnd) + sizeof(node->children);
    RadixNode **node_children = node->children;
    for(int i=0;i<128;i++) {
        if(node_children[i] != NULL) {
            size += getMemoryUsage(node_children[i]);
        }
    }
    return size;
}

// free the tree from memory after usage
static void freeTree(RadixNode *node, bool is_root=true) {
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
static void del_RadixNode(PyObject *obj) {
    freeTree((RadixNode*)PyCapsule_GetPointer(obj, "RadixNode"));
    free(PyCapsule_GetPointer(obj, "RadixNode"));
}

/* Utility functions */
static RadixNode *PyRadix_AsRadixNode(PyObject *obj) {
    return (RadixNode *) PyCapsule_GetPointer(obj, "RadixNode");
}

static PyObject *PyRadix_FromRadixNode(RadixNode *node, int must_free) {
    return PyCapsule_New(node, "RadixNode", must_free ? del_RadixNode : NULL);
}

/* Create Radix Tree */
static PyObject *py_create(PyObject *self, PyObject *args) {
    RadixNode *node = create(false, "<root>");
    return PyRadix_FromRadixNode(node, 1);
}

/* Insert word into Radix Tree */
static PyObject *py_insert(PyObject *self, PyObject *args) {
    RadixNode *root;
    PyObject *py_root;
    const char *text;
    
    if (!PyArg_ParseTuple(args, "Os", &py_root, &text)) {
        return NULL;
    }
    if (!(root = PyRadix_AsRadixNode(py_root))) {
        return NULL;
    }
    insert(root, text);
    Py_RETURN_NONE;
}

/* Check whether the full given text exists in Radix Tree */
static PyObject *py_matchFull(PyObject *self, PyObject *args) {
    RadixNode *root;
    PyObject *py_root;
    const char *text;
    
    if (!PyArg_ParseTuple(args, "Os", &py_root, &text)) {
        return NULL;
    }
    if (!(root = PyRadix_AsRadixNode(py_root))) {
        return NULL;
    }
    int result = matchFull(root, text);
    return Py_BuildValue("i", result);
}

/* Check whether any substring of the given text exists in Radix Tree */
static PyObject *py_matchSub(PyObject *self, PyObject *args) {
    RadixNode *root;
    PyObject *py_root;
    const char *text;
    
    if (!PyArg_ParseTuple(args, "Os", &py_root, &text)) {
        return NULL;
    }
    if (!(root = PyRadix_AsRadixNode(py_root))) {
        return NULL;
    }
    int result = matchSub(root, text);
    return Py_BuildValue("i", result);
}

/* Get Memory Usage of Radix Tree */
static PyObject *py_getMemoryUsage(PyObject *self, PyObject *args) {
    RadixNode *root;
    PyObject *py_root;
    
    if (!PyArg_ParseTuple(args, "O", &py_root)) {
        return NULL;
    }
    if (!(root = PyRadix_AsRadixNode(py_root))) {
        return NULL;
    }
    int memSize = getMemoryUsage(root);
    return Py_BuildValue("i", memSize);
}

/* Visualize Radix Tree for Debug */
static PyObject *py_visualize(PyObject *self, PyObject *args) {
    RadixNode *root;
    PyObject *py_root;
    
    if (!PyArg_ParseTuple(args, "O", &py_root)) {
        return NULL;
    }
    if (!(root = PyRadix_AsRadixNode(py_root))) {
        return NULL;
    }
    visualize(root, 0);
    Py_RETURN_NONE;
}

/* Free the Tree from Memory after Usage */
static PyObject *py_free(PyObject *self, PyObject *args) {
    RadixNode *root;
    PyObject *py_root;
    
    if (!PyArg_ParseTuple(args, "O", &py_root)) {
        return NULL;
    }
    if (!(root = PyRadix_AsRadixNode(py_root))) {
        return NULL;
    }
    freeTree(root);
    Py_RETURN_NONE;
}

/* Module method table */
static PyMethodDef RadixMethods[] = {
    {"create", py_create, METH_VARARGS, "Create Radix Tree"},
    {"insert", py_insert, METH_VARARGS, "Insert word into Radix Tree"},
    {"matchFull", py_matchFull, METH_VARARGS, "Check whether the full given text exists in Radix Tree"},
    {"matchSub", py_matchSub, METH_VARARGS, "Check whether any substring of given text exists in Radix Tree"},
    {"getMemoryUsage", py_getMemoryUsage, METH_VARARGS, "Get memory usage of Radix Tree"},
    {"visualize", py_visualize, METH_VARARGS, "Visualize Radix Tree for Debug"},
    {"free", py_free, METH_VARARGS, "Free the Tree from Memory after Usage"},
    {NULL, NULL, 0, NULL}
};

/* Module definition structure */
static struct PyModuleDef radixmodule = {
    PyModuleDef_HEAD_INIT,   /* m_base */
    "radix",                 /* name of module */
    "Radix Tree",            /* module documentation, may be NULL */
    -1,                      /* size of per-interpreter state or -1 */
    RadixMethods             /* method table */
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_radix(void) {
    Py_Initialize();
    return PyModule_Create(&radixmodule);
}
