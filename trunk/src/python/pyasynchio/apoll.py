# Copyright (c) 2005 Vladimir Sukhoy

# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is furnished 
# to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all 
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
# PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

__doc__="""Module which implements apoll class wrapper"""


from _pyasynchio import apoll as _apoll, get_sock_family, get_sock_type, get_sock_proto
import socket

class apoll(_apoll):
    """\
    The apoll class is responsible for both initiating asynchronous operations
    and delivering their results. The results are delivered via simple 
    interface which does not bring extra OOP burden for class user. Results
    are delivered as simple dictionaries and can be either processed by some
    object oriented logic or by more primitive approaches. That is why this
    class is called apoll, or "asynchronous poll". The approach taken is similar
    to the approach of poll object in module select. Nevertheless, AIO which
    apoll implements is very much different from nonblocking I/O which is 
    implemented in select module.
    """
    def __init__(self, max_threads = 0):
        """__init__([max_threads=0])
        Initialize apoll object. max_threads parameter specifies how
        many threads are able to poll this object simultaneously.
        """
        _apoll.__init__(self, max_threads)

    def accept(self, lsock, act = None):
        """accept(self, lsock [,act])
        Start asynchronous accept operation on listening socket. 
        act, which defaults to None is asynchronous completion token for this operation. 
        Upon completion, the result dictionary has following keys:
        "type" maps to string  "accept",
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "listen_socket" maps to listening socket (lsock parameter of accept), 
        "accept_socket" maps to socket which is the result of accept op, 
        "act" maps to asynchronous completion token of this operation (act parameter of accept).
        """
        lsock_family = get_sock_family(lsock._sock)
        lsock_type = get_sock_type(lsock._sock)
        lsock_proto = get_sock_proto(lsock._sock)
        asock = socket.socket(lsock_family, lsock_type, lsock_proto)
        return _apoll.accept(self, lsock._sock, asock._sock, lsock, asock, act)

    def connect(self, sock, addr, act = None):
        """connect(self, sock, addr, [, act])
        Start asynchronous connect operation on socket sock.
        If sock is SOCK_STREAM socket, then if it is not bound, then it will be 
        bound to random address (just like synchronous connect does).
        act, which defaults to None represents asynchronous completion token for this operation.
        Upon completion the result dictionary has following keys:
        "type" maps to string "connect",
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "socket" maps to socket which was connected (sock parameter of apoll.connect), 
        "addr" maps to addr object which was passed to this method (addr parameter of apoll.connect)
        "act" maps to asynchronous completion token for this operation (act parameter of apoll.connect).
        """
        if get_sock_type(sock._sock) == socket.SOCK_STREAM:
            try:
                sock.bind(('', 0))
            except socket.error:
                pass
        return _apoll.connect(self, sock._sock, sock, addr, act)

    def send(self, sock, data, flags = 0, act = None):
        """send(self, sock, data [, flags, act])
        Start asynchronous send operation on socket, flags have the same meaning as in usual socket.send method.
        act, which defaults to None represents asynchronous completion token for this operation.
        Upon completion the result notification dictionary has following keys:
        "type" maps to string "send", 
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "socket" maps to socket which was used in send operation (sock parameter of apoll.send method), 
        "data" maps to data which was intended to be sent (data parameter of apoll.send method), 
        "flags" maps to flags for send operation (flags parameter of apoll.send method),
        "count" maps to integer which specifies how many bytes of data was sent,
        "act" maps to asynchronous completion token for this operation (act parameter of apoll.send method).
        """
        return _apoll.send(self, sock._sock, sock, data, flags, act)

    def sendto(self, sock, addr, data, flags = 0, act = None):
        """sendto(self, sock, addr, data [, flags, act])
        Start asynchronous sendto operation on socket, flags have the same meaning as in synchronous socket.sendto method.
        act, which defaults to None represents asynchronous completion token for this operation.
        Upon completion the result notification dictionary has following keys:
        "type" maps to string "sendto",
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "socket" maps to socket which was used in sendto operation (sock parameter of apoll.sendto method),
        "data" maps to data which was intended to be sent (data parameter of apoll.sendto method),
        "flags" maps to flags for this particular sendto operation (flags parameter of apoll.sendto),
        "count" maps to integer which specifies how many bytes of data was sent,
        "addr" maps to address to which data was intended to be sent (addr parameter of apoll.sendto),
        "act" maps to asynchronous completion token for this operation (act parameter of apoll.sendto)        
        """
        return _apoll.sendto(self, sock._sock, sock, addr, data, flags, act)

    def recv(self, sock, bufsize, flags = 0, act = None):
        """recv(self, sock, bufsize [, flags , act])
        Start asynchronous recv operation on socket.
        act, which defaults to None represents asynchronous completion token for this operation.
        Upon completion the result notification dictionary has following keys:
        "type" maps to string "recv",
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "socket" maps to socket which was used in recv operation (sock parameter of apoll.recv method),
        "bufsize" maps to buffer size for data (bufsize parameter of apoll.recv),
        "flags" maps to flags for this particular recv operation (flags parameter of apoll.recv),
        "act" maps to asynchronous completion token for this operation (act parameter of apoll.recv).
        If recv is successful, then result notification dictionary has one more key:
        "data" which maps to data being received (of size <= bufsize).
        """
        return _apoll.recv(self, sock._sock, sock, bufsize, flags, act)

    def recvfrom(self, sock, bufsize, flags = 0, act = None):
        """recvfrom(self, sock, bufsize, [, flags, act])
        Start asynchronous recvfrom operation on socket.
        act, which defaults to None represents asynchronous completion token for this particular operation.
        Upon completion the result notification dictionary has following keys:
        "type" maps to string "recvfrom",
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "socket" maps to socket which was used in recvfrom operation (sock parameter of apoll.recvfrom method),
        "bufsize" maps to buffer size for data (bufsize parameter of apoll.recvfrom),
        "flags" maps to flags for this particular recvfrom operation (flags parameter of apoll.recvfrom),
        "act" maps to asynchronous completion token for this operation (act parameter of apoll.recvfrom).
        If recvfrom is successful, then result notification dictionary has one more key:
        "data" which maps to data being received (of size <= bufsize).
        """
        return _apoll.recvfrom(self, sock._sock, sock, bufsize, flags, act)

    def poll(self, timeout = None):
        """poll(self [, timeout]) -> list_of_result_dictionaries
        Poll asynchronous results. timeout (in milliseconds) specifies how much time to wait for first 
        result. If omitted or negative, then waiting is infinite. Returns list of dictionaries with
        string keys (all of them have "type" key which maps to string which specifies the name of
        operation, like "send", "recv", "read", "write", etc. matching the name of appropriate method). Each
        of dictionaries also contains "act" key which maps to asynchronous completion token of particular
        operation.
        """
        if timeout == None or timeout < 0:
            timeout = -1    
        return _apoll.poll(self, timeout)
    
    def cancel(self, obj):
        """cancel(self, obj)
        Makes an attempt to cancel pending AIO operation on file or socket (obj). This method is
        unreliable, e.g. on windows it cancels only AIO operations which were issued by current thread.
        Exercise caution.
        """
        return _apoll.cancel(self, obj)

    def read(self, file, offset, bufsize, act = None):
        """read(self, file, offset, size [, act])
        Start asynchronous read operation on file.
        act, which defaults to None represents asynchronous completion token for this particular operation.
        Upon completion the result notification dictionary has following keys:
        "type" maps to string "read",
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "file" maps to file object (file parameter of apoll.read),
        "offset" maps to offset of read (offset parameter of apoll.read),
        "bufsize" maps to buffer size, that is number of bytes to read (bufsize parameter of apoll.read),
        "data" maps to data which was read,
        "act" maps to asynchronous completion token for this operation.
        """
        return _apoll.read(self, file, offset, bufsize, act)

    def write(self, file, offset, data, act = None):
        """write(self, file, offset, data [, act])
        Start asynchronous write operation on file.
        act, which defaults to None represents asynchronous completion token for this particular operation.
        Upon completion the result notification dictionary has following keys:
        "type" maps to string "write",
        "success" maps to 1 if operation succeeded and 0 otherwise,
        "file" maps to file object (file parameter of apoll.write),
        "offset" maps to offset of write (offset parameter of apoll.write),
        "data" maps to data which had to be written,
        "count" maps to number of bytes that were written,
        "act" maps to asynchronous completion token for this operation.
        """
        return _apoll.write(self, file, offset, data, act)
        