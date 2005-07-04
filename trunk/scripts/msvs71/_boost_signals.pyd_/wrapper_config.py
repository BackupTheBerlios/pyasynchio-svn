from os import path, chdir, getcwd

# move to top-level
out = path.abspath('boost-signals-python.cpp')
chdir(path.join('..', '..', '..'))

includes = map(path.abspath, ['include', 'vendor/boost'])
files = map(path.abspath, 
['src/pyste/boost/signals/signals.pyste'])
module = '_boost_signals'



