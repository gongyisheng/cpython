#include <Python.h>
#include <string>
#include <queue>

// Radix node is a kind of compressed Radix node
// Radix node struct
typedef struct RadixNode {
    bool isEnd;
    std::string word;
    RadixNode *children[128];
} RadixNode;

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
        std::string existing_word = curr_children[c]->word;
        // compare & see if the perfix of existing word and the new word are the same
        int j = 0;
        while(j<existing_word.size() && i<text.size() && existing_word[j]==text[i]) {
            j++;
            i++;
        }

        // if reach the end of the existing word
        if(j==existing_word.size()) {
            curr = curr_children[c];
        }
        else { // if not reach the end of the existing word
            // create a new node to be appended to the existing node
            RadixNode *new_node = new RadixNode();
            new_node->word = existing_word.substr(j, existing_word.size()-j);
            new_node->isEnd = curr_children[c]->isEnd;
            *new_node->children = *curr_children[c]->children;
            // update the existing node
            RadixNode *existing_node = curr_children[c];
            existing_node->word = existing_word.substr(0, j);
            existing_node->isEnd = false;
            existing_node->children[(int)new_node->word[0]] = new_node;
            // if the new word is not longer than the existing word
            if(i==text.size()) {
                curr_children[c]->isEnd = true;
            } 
            else { // if the new word is longer than the existing word
                RadixNode *new_node2 = new RadixNode();
                new_node2->word = text.substr(i);
                new_node2->isEnd = true;
                curr_children[c]->children[(int)new_node2->word[0]] = new_node2;
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

// calc memory usage of radix tree
static int getMemoryUsage(RadixNode* root) {
    int size = sizeof(RadixNode);
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

/* Module method table */
static PyMethodDef RadixMethods[] = {
    {"create", py_create, METH_VARARGS, "Create Radix Tree"},
    {"insert", py_insert, METH_VARARGS, "Insert word into Radix Tree"},
    {"isPartOf", py_isPartOf, METH_VARARGS, "Check if part of text exists in Radix Tree"},
    {"getMemoryUsage", py_getMemoryUsage, METH_VARARGS, "Get memory usage of Radix Tree"},
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
