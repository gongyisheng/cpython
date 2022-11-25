# Radix Tree Python Extension
### Prerequisite
- python >= 3.10.0
- clang >= 14.0.0
- clone this cpython folk
`git clone https://github.com/gongyisheng/cpython.git`
- checkout to target branch  
`git checkout radix-extension`
### Usage
- setup python binding  
`python setup_radix.py install`
- develop test scripts  
```
import radix  

# create a radix tree
tree = radix.create()

# insert words to find
radix.insert(tree, "foo")
radix.insert(tree, "bar")
radix.insert(tree, "foobar")

# visualize tree
radix.visualize(tree)
# Node [word=,isEnd=0]
#     Node [word=bar,isEnd=1]
#     Node [word=foo,isEnd=1]
#         Node [word=bar,isEnd=1]

# find if a given text contains any substring that can be found in radix tree  
radix.isPartOf(tree, "randominvalidtext") # 0
radix.isPartOf(tree, "randomtextfoorandomtext") # 1

# free the tree from memory after usage
radix.free(tree)
```