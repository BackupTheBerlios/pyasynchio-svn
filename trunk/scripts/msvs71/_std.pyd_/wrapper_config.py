from os import path, chdir, getcwd

# move to top-level
out = path.abspath('std-python.cpp')
chdir(path.join('..', '..', '..'))

includes = map(path.abspath, ['include', 'vendor/boost', 'vendor/ACE'])
files = map(path.abspath, 
['src/pyste/std/pair.pyste'])
module = '_std'



