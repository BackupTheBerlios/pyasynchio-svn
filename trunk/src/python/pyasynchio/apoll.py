from _pyasynchio import apoll as _apoll, get_sock_family, get_sock_type, get_sock_proto
import socket

class apoll(_apoll):
    def __init__(self):
        _apoll.__init__(self)

    def accept(self, lsock, act = None):
        lsock_family = get_sock_family(lsock._sock)
        lsock_type = get_sock_type(lsock._sock)
        lsock_proto = get_sock_proto(lsock._sock)
        asock = socket.socket(lsock_family, lsock_type, lsock_proto)
        return _apoll.accept(self, lsock._sock, asock._sock, lsock, asock, act)

    def connect(self, sock, addr, act = None):
        if get_sock_type(sock._sock) == socket.SOCK_STREAM:
            try:
                sock.bind(('', 0))
            except socket.error:
                pass
        return _apoll.connect(self, sock._sock, sock, addr, act)

    def send(self, sock, data, flags = 0, act = None):
        return _apoll.send(self, sock._sock, sock, data, flags, act)

    def sendto(self, sock, addr, data, flags = 0, act = None):
        return _apoll.sendto(self, sock._sock, sock, addr, data, flags, act)

    def recv(self, sock, bufsize, flags = 0, act = None):
        return _apoll.recv(self, sock._sock, sock, bufsize, flags, act)

    def recvfrom(self, sock, bufsize, flags = 0, act = None):
        return _apoll.recvfrom(self, sock._sock, sock, bufsize, flags, act)

    def poll(self, timeout = None):
        if timeout == None or timeout < 0:
            timeout = -1    
        return _apoll.poll(self, timeout)
    
    def cancel(self, obj):
        return _apoll.cancel(self, obj)

    def read(self, file, offset, size, act = None):
        return _apoll.read(self, file, offset, size, act)

    def write(self, file, offset, data, act = None):
        return _apoll.write(self, file, offset, data, act)
        