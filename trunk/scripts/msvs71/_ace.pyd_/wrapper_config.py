from os import path, chdir, getcwd

# move to top-level
out = path.abspath('ace-python.cpp')
chdir(path.join('..', '..', '..'))

includes = map(path.abspath, ['include', 'vendor/boost', 'vendor/ACE'])
files = map(path.abspath, 
['src/pyste/ace/INET_Addr.pyste'
,'src/pyste/ace/Time_Value.pyste'])
module = '_ace'



