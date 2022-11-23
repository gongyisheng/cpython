# setup.py
from distutils.core import setup, Extension

module1 = Extension('trie',
                    sources = ['trie.cpp'])

setup (name = 'trie',
       version = '0.0.1',
       description = 'Trie tree implementation',
       ext_modules = [module1])