import radix

def test_add_word():
    tree = radix.create()
    radix.insert(tree, "abc")
    radix.insert(tree, "abd")
    radix.insert(tree, "abca")
    radix.visualize(tree)

def test_find_word():
    tree = radix.create()
    radix.insert(tree, "abc")
    radix.insert(tree, "abd")
    radix.insert(tree, "abca")
    assert(radix.matchFull(tree, "abc") == True)
    assert(radix.matchFull(tree, "abd") == True)
    assert(radix.matchFull(tree, "abca") == True)
    assert(radix.matchFull(tree, "ab") == False)
    assert(radix.matchFull(tree, "abcb") == False)
    assert(radix.matchFull(tree, "abdashdka") == False)
    assert(radix.matchSub(tree, "abc") == True)
    assert(radix.matchSub(tree, "abd") == True)
    assert(radix.matchSub(tree, "abca") == True)
    assert(radix.matchSub(tree, "ab") == False)
    assert(radix.matchSub(tree, "abcb") == True)
    assert(radix.matchSub(tree, "dabababababababababababc") == True)

if __name__ == '__main__':
    test_add_word()
    test_find_word()
    