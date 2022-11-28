# setup.py
from distutils.core import setup, Extension

module1 = Extension('kmp',
                    sources = ['kmp.cpp'])

setup (name = 'kmp',
       version = '0.0.1',
       description = 'KMP implementation',
       ext_modules = [module1])