import pyasynchio
import pyasynchio.echo
import sys

pro = pyasynchio.Proactor()
port = int(sys.argv[1])
#ac = pyasynchio.AcceptContext()
echo = pyasynchio.echo.Echo(pro)
pro.open_stream_accept(echo, ('', port))

while True:
    pro.handle_events_forever()