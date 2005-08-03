try:
    from select import poll, error, POLLIN, POLLOUT
except ImportError:
    from selectpoll import poll, error, POLLIN, POLLOUT
import socket
import sys

acceptor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
port = int(sys.argv[1])
acceptor.bind(('', port))
acceptor.setblocking(0)
acceptor.listen(15)
afd = acceptor.fileno()
poller = poll()
poller.register(afd, POLLIN)
lut = {}
ldata = {}

while True:
    events = poller.poll()
    for event in events:
        if event == (afd, POLLIN):
            sock = acceptor.accept()[0]
            sock.setblocking(0)
            poller.register(sock.fileno(), POLLIN | POLLOUT)
            lut[sock.fileno()] = sock
        elif event[1] == POLLIN:
            sock = lut[event[0]]
            data = sock.recv(102400)
            if data == 'shutdown':
                del lut[event[0]]
                poller.unregister(event[0])
                sock.close()
            else:
                if not ldata.has_key(event[0]):
                    ldata[event[0]] = ''
                ldata[event[0]] += data

        elif event[1] == POLLOUT:
            if lut.has_key(event[0]):
                sock = lut[event[0]]
                if ldata.has_key(event[0]):
                    sock.send(ldata[event[0]])
                    del ldata[event[0]]
