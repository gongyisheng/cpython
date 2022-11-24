# setup.py
from distutils.core import setup, Extension

module1 = Extension('radix',
                    sources = ['radix.cpp'])

setup (name = 'radix',
       version = '0.0.1',
       description = 'Radix tree implementation',
       ext_modules = [module1])