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

import pyasynchio
import unittest
import socket

class StreamEcho:
    def __init__(self, lsock):
        # Create apoll object
        apoll = pyasynchio.apoll()
        self.lsock = lsock
        self.apoll = apoll
        # Initiate asynchronous accept on the listening socket
        apoll.accept(lsock)

    def poll(self, timeout = None):
        # poll asynch results
        events = self.apoll.poll(timeout)

        for name, success, more in events:
            if name == 'accept':
                if success:
                    # initiate receiving of data on accepted socket
                    self.apoll.recv(more['accept_socket'], 1024)
                    # initiate another accept, so more clients can connect
                    self.apoll.accept(self.lsock)
                else:
                    # close socket which was meant to represent new connection
                    more['accept_socket'].close()
            elif name == 'recv':
                if success and more['data'] != '':
                    # echo that data back
                    self.apoll.send(more['socket'], more['data'])
                    # read more data
                    self.apoll.recv(more['socket'], 1024)
                else:
                    # if there is some error, or connection was closed
                    # , then we also close the socket
                    more['socket'].close()
                
class DgramEcho:
    def __init__(self, sock):
        apoll = pyasynchio.apoll()
        self.apoll = apoll
        self.sock = sock
        # initiate asynchronous dgram recv
        apoll.recvfrom(sock, 65536)

    def poll(self, timeout = None):
        # poll results
        for name, success, more in self.apoll.poll(timeout):
            if name == 'recvfrom' and success:
                # echo data back
                self.apoll.sendto(self.sock, more['addr'], more['data'])
                # read more data asynchronously
                self.apoll.recvfrom(self.sock, 65536)

class TestStreamEcho(unittest.TestCase):
    def setUp(self):
        self.lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.port = 40264
        self.lsock.bind(('', self.port))
        self.lsock.listen(5)
        self.echo = StreamEcho(self.lsock)
        self.done = False
        import thread
        thread.start_new_thread(self.thr_func, ())
    
    def thr_func(self):
        while not self.done:
            self.echo.poll()

    def tearDown(self):
        self.done = True

    def test_it(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(('127.0.0.1', self.port))
        data = 'data for sending'
        self.assert_(sock.send(data) == len(data))
        self.assert_(sock.recv(len(data)) == data)

        biggerstuff = 'stuffy' * 10000

        echoed = ''
        max_portion_size = 2048
        while(len(echoed) != len(biggerstuff)):
            portion_size = min(max_portion_size, len(biggerstuff) - len(echoed))
            portion = biggerstuff[len(echoed):len(echoed) + portion_size]
            sock.send(portion)
            recvbuf = ''
            while len(recvbuf) != len(portion):
                recvbuf += sock.recv(len(portion))
            echoed += recvbuf
        
        self.assert_(echoed == biggerstuff)

class TestDgramEcho(unittest.TestCase):
    def setUp(self):
        self.lsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.port = 40265
        self.lsock.bind(('', self.port))
        self.echo = DgramEcho(self.lsock)
        self.done = False
        import thread
        thread.start_new_thread(self.thr_func, ())

    def thr_func(self):
        while not self.done:
            self.echo.poll()

    def test_it(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        data = 'data for sending'
        self.assert_(sock.sendto(data, ('127.0.0.1', self.port)) == len(data))
        self.assert_(sock.recvfrom(len(data))[0] == data)

        biggerstuff = 'stuffy' * 10000

        echoed = ''
        max_portion_size = 2048
        while(len(echoed) != len(biggerstuff)):
            portion_size = min(max_portion_size, len(biggerstuff) - len(echoed))
            portion = biggerstuff[len(echoed):len(echoed) + portion_size]
            sock.sendto(portion, ('127.0.0.1', self.port))
            recvbuf = ''
            while len(recvbuf) != len(portion):
                recvbuf += sock.recvfrom(len(portion))[0]
            echoed += recvbuf
        
        self.assert_(echoed == biggerstuff)


suite = unittest.TestSuite()
suite.addTest(unittest.makeSuite(TestStreamEcho))
suite.addTest(unittest.makeSuite(TestDgramEcho))