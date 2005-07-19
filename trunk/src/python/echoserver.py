import pyasynchio
import pyasynchio.echo

pro = pyasynchio.Proactor(False)
port = 40274
#ac = pyasynchio.AcceptContext()
echo = pyasynchio.echo.Echo(pro)
pro.open_stream_accept(echo, ('', port))

while True:
    pro.handleEvents((0,100000))