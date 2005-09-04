#!/user/bin/env python
version = '0.9.2'

from distutils.core import setup, Extension
from distutils.sysconfig import get_python_inc, PREFIX
from distutils.fancy_getopt import getopt, FancyGetopt
import os
from os.path import join

if os.environ.has_key('PY_MODULES_DIR'):
    modules_dir = os.environ['PY_MODULES_DIR']
else:
    modules_dir = join(PREFIX, 'Modules')

include_dirs = ['include', join('include', 'mscrt'), get_python_inc()
                    , modules_dir]
src_dir = join('src', 'cplusplus', 'pyasynchio')
libs = ['mswsock', 'ws2_32']

setup(name = 'pyasynchio'
        , version = version
        , description = 'Python asynchronous I/O library'
        , author = 'Vladimir Sukhoy'
        , license = 'MIT license'
        , author_email = 'randolphu@users.berlios.de'
        , url = 'http://developer.berlios.de/projects/pyasynchio'
        , packages = ['pyasynchio', 'pyasynchio.test']
        , package_dir = {'pyasynchio' : join('src', 'python', 'pyasynchio') 
                        ,'pyasynchio.test' : join('src', 'python', 'pyasynchio', 'test')}
        , ext_modules = [Extension('pyasynchio._pyasynchio'
                            , [ join(src_dir, 'py_apoll.cpp')
                                , join(src_dir, 'py_apoll_aio.cpp')
                                , join(src_dir, 'socketmodule_stuff.cpp') ]
                            , include_dirs = include_dirs 
                            , libraries = libs)])
