#include <Python.h>
#include <string>
#include <queue>
#include <iostream>

// Radix node is a kind of compressed Trie node
// Visualization: https://www.cs.usfca.edu/~galles/visualization/RadixTree.html

// Radix node struct
typedef struct RadixNode {
    bool isEnd;
    std::string word;
    RadixNode *children[128];
} RadixNode;

// swap children between two radix nodes
// TODO: this func can be improved if node->children is a pointer to array. 
static void swapChildren(RadixNode *a, RadixNode *b){
    for(int i=0;i<128;i++){
        RadixNode *tmp = a->children[i];
        a->children[i] = b->children[i];
        b->children[i] = tmp;
    }
}

// insert a word to radix tree
static void insert(RadixNode* root, const std::string &text) {
    RadixNode* curr = root;
    int i = 0;
    while(i<text.size()) {
        int c = (int)text[i];
        RadixNode **curr_children = curr->children;
        // if the current node has no child with `c` as perfix
        if (curr_children[c] == NULL) {
            RadixNode *nnode = new RadixNode();
            nnode->word = text.substr(i, text.size()-i);
            nnode->isEnd = true;
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
            RadixNode *lnode = new RadixNode();
            RadixNode *pnode = curr_children[c];
            lnode->word = curr_word.substr(j, curr_word.size()-j);
            lnode->isEnd = curr_children[c]->isEnd;
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
                RadixNode *rnode = new RadixNode();
                rnode->word = text.substr(i);
                rnode->isEnd = true;
                curr_children[c]->children[(int)rnode->word[0]] = rnode;
            }
            break;
        }
    }
}

// find whether any substring in text is in radix tree
static int isPartOf(RadixNode* root, const std::string &text){
    std::queue<std::pair<RadixNode*, int> > q;
    RadixNode **root_children = root->children;
    for (int i=0;i<text.size();i++) {
        char c = (int)text[i];
        if(q.size()!=0){
            int size = q.size();
            for(int j=0;j<size;j++){
                std::pair<RadixNode*, int> p = q.front();
                RadixNode *node = p.first;
                RadixNode **node_children = node->children;
                int index = p.second;
                q.pop();
                if(node->word[index]==text[i]) {
                    // if cursor is at the end of the word
                    if(index==node->word.size()-1) {
                        // reach the end
                        if(node->isEnd) {
                            return true;
                        }
                        // has next node
                        else if(node_children[c] != NULL) {
                            q.push(std::make_pair(node_children[c], 0));
                        }
                    }
                    else {
                        q.push(std::make_pair(node, index+1));
                    }
                }
            }
        }
        if(root_children[c] != NULL){
            q.push(std::make_pair(root_children[c], 1));
        }
    }
    return false;
}

// visualize
static void visualize(RadixNode* node, int level=0) {
    for(int i=0;i<level*4;i++) {
        std::cout << " ";
    }
    std::cout << "Node [word=" << node->word << ",isEnd=" << node->isEnd << "]\n";
    for(int i=0;i<128;i++) {
        if(node->children[i] != NULL) {
            visualize(node->children[i], level+1);
        }
    }
}

// calc memory usage of radix tree
static int getMemoryUsage(RadixNode* root) {
    int size = sizeof(root) + sizeof(root->word) + sizeof(root->isEnd) + sizeof(root->children);
    RadixNode **root_children = root->children;
    for(int i=0;i<128;i++) {
        if(root_children[i] != NULL) {
            size += getMemoryUsage(root_children[i]);
        }
    }
    return size;
}

/* Destructor function for points */
static void del_RadixNode(PyObject *obj) {
    free(PyCapsule_GetPointer(obj,"RadixNode"));
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
    RadixNode *node = new RadixNode();
    node->isEnd = false;
    memset(node->children, NULL, 128);
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

/* Check if part of text exists in Radix Tree */
static PyObject *py_isPartOf(PyObject *self, PyObject *args) {
    RadixNode *root;
    PyObject *py_root;
    const char *text;
    
    if (!PyArg_ParseTuple(args, "Os", &py_root, &text)) {
        return NULL;
    }
    if (!(root = PyRadix_AsRadixNode(py_root))) {
        return NULL;
    }
    int result = isPartOf(root, text);
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

/* Module method table */
static PyMethodDef RadixMethods[] = {
    {"create", py_create, METH_VARARGS, "Create Radix Tree"},
    {"insert", py_insert, METH_VARARGS, "Insert word into Radix Tree"},
    {"isPartOf", py_isPartOf, METH_VARARGS, "Check if part of text exists in Radix Tree"},
    {"getMemoryUsage", py_getMemoryUsage, METH_VARARGS, "Get memory usage of Radix Tree"},
    {"visualize", py_visualize, METH_VARARGS, "Visualize Radix Tree for Debug"},
    {NULL, NULL, 0, NULL}
};

/* Module definition structure */
static struct PyModuleDef radixmodule = {
    PyModuleDef_HEAD_INIT,  /* m_base */
    "radix",                 /* name of module */
    "Radix Tree",            /* module documentation, may be NULL */
    -1,                     /* size of per-interpreter state or -1 */
    RadixMethods             /* method table */
};

/* Module initialization function */
PyMODINIT_FUNC PyInit_radix(void) {
    Py_Initialize();
    return PyModule_Create(&radixmodule);
}
