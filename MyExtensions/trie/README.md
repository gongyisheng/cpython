# Radix Tree Python Extension
### Background
Trie tree is a kind of prefix tree to locate specific keys from within a set  
[Trie tree wiki](https://en.wikipedia.org/wiki/Trie)  
[Visualization simulator](https://www.cs.usfca.edu/~galles/visualization/Trie.html)
### Prerequisite
- python >= 3.10.0
- clang >= 14.0.0
- clone this cpython folk  
`git clone https://github.com/gongyisheng/cpython.git`
- checkout to target branch  
`git checkout trie-extension`
### Usage
- setup python binding  
`python setup_trie.py install`
- develop test scripts  
```
import trie  

# create a trie tree
tree = trie.create()

# insert words to find
trie.insert(tree, "foo")
trie.insert(tree, "bar")
trie.insert(tree, "foobar")

# visualize tree
trie.visualize(tree)
# Node [character=/,isEnd=0]
#     Node [character=b,isEnd=0]
#         Node [character=a,isEnd=0]
#             Node [character=r,isEnd=1]
#     Node [character=f,isEnd=0]
#         Node [character=o,isEnd=0]
#             Node [character=o,isEnd=1]
#                 Node [character=b,isEnd=0]
#                     Node [character=a,isEnd=0]
#                         Node [character=r,isEnd=1]

# find if a given text contains any substring that can be found in trie tree  
trie.isPartOf(tree, "random invalid text") # 0
trie.isPartOf(tree, "random text foo random text") # 1

# get memory usage of tree
trie.getMemoryUsage(tree) # 10330

# free the tree from memory after usage
trie.free(tree)
```
### Known issue
- Only support ASCII character for performance reasons. String with UTF8/UTF16 encoding should be parsed into ASCII before using trie.