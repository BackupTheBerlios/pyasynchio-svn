import pyasynchio
import pyasynchio.echo

pro = pyasynchio.Proactor(False)
port = 40274
ac = pyasynchio.AcceptContext()
echo = pyasynchio.echo.Echo(pro, ac)
pro.accept(ac, ('', port))

while True:
    pro.handleEvents((0,100000))