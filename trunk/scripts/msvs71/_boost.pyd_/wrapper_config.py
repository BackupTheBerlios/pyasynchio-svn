from os import path, chdir, getcwd

# move to top-level
out = path.abspath('boost-python.cpp')
chdir(path.join('..', '..', '..'))

includes = map(path.abspath, ['include', 'vendor/boost'])
files = map(path.abspath, 
['src/pyste/boost/noncopyable.pyste' 
,'src/pyste/boost/function_base.pyste'
])
module = '_boost'



