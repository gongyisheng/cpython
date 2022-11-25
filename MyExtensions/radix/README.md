# Radix Tree Python Extension
### Background
Radix tree is a kind of compressed trie tree providing same performance as trie tree but much less memory usage  
[Radix tree wiki](https://en.wikipedia.org/wiki/Radix_tree)  
[Visualization simulator](https://www.cs.usfca.edu/~galles/visualization/RadixTree.html)
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
# Node [address=0x13383e200, word=<root>, isEnd=0]
#     Node [address=0x134009c00, word=bar, isEnd=1]
#     Node [address=0x12300a000, word=foo, isEnd=1]
#         Node [address=0x113808200, word=bar, isEnd=1]

# find whether the full given text can be found in radix tree  
radix.matchFull(tree, "foooooooo") # 0
radix.matchFull(tree, "foo") # 1

# find whether any substring of the given text can be found in radix tree  
radix.matchSub(tree, "random invalid text") # 0
radix.matchSub(tree, "random text foo random text") # 1

# get memory usage of the tree
radix.getMemoryUsage(tree) # 4228

# free the tree from memory after usage
radix.free(tree)
```
### Known issue
- Only support ASCII character for performance reasons. String with UTF8/UTF16 encoding should be parsed into ASCII before using trie.
- If you need to continue to reduce the memory usage of trie tree, you can try Radix tree, Burst-trie tree or [HAT-trie tree](https://github.com/Tessil/hat-trie). [Detailed Doc here](https://tessil.github.io/2017/06/22/hat-trie.html)

### ToDo
- Add cmake, test cases, ci github workflow
- Add pyi file for c interfaces, write a Trie Module in python
- Experiment with KMP algorithm to improve performance (but may at a cost of large memory usage)
- \[optional\]Add radix.delete() to remove a word from radix tree