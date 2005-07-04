from os import path, chdir, getcwd

# move to top-level
out = path.abspath('boost-python.cpp')
chdir(path.join('..', '..', '..'))

includes = map(path.abspath, ['include', 'vendor/boost'])
files = map(path.abspath, 
['src/pyste-files/boost/noncopyable.pyste' 
,'src/pyste-files/boost/function_base.pyste'
])
module = '_boost'



