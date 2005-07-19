from os import path, chdir, getcwd

# move to top-level
out = path.abspath('pyasynchio-python')
chdir(path.join('..', '..', '..'))

includes = map(path.abspath, ['include'
, 'vendor/boost'
, 'vendor/ACE'
, 'vendor/Python/PC'
, 'vendor/Python/Include'])
files = map(path.abspath, 
['src/pyste/pyasynchio/Proactor.pyste'
,'src/pyste/pyasynchio/Contexts.pyste'
,'src/pyste/pyasynchio/Results.pyste'
,'src/pyste/pyasynchio/Signals.pyste'
,'src/pyste/pyasynchio/Handlers.pyste'
,'src/pyste/pyasynchio/buf.pyste']

)
module = '_pyasynchio'
multiple = True