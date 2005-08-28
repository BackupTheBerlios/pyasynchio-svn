import pyasynchio
import unittest
import socket

class StreamEcho:
    def __init__(self, lsock):
        apoll = pyasynchio.apoll()
        self.lsock = lsock
        self.apoll = apoll
        apoll.accept(lsock)

    def poll(self, timeout = None):
        events = self.apoll.poll(timeout)

        for event in events:
            if event['type'] == 'accept':
                if event['success'] == True:
                    self.apoll.recv(event['accept_socket'], 1024)
                    self.apoll.accept(self.lsock)
                else:
                    event['accept_socket'].close()
            elif event['type'] == 'recv':
                if event['success'] and event['data'] != '':
                    self.apoll.send(event['socket'], event['data'])
                    self.apoll.recv(event['socket'], 1024)
                else:
                    event['socket'].close()
                
class DgramEcho:
    def __init__(self, sock):
        apoll = pyasynchio.apoll()
        self.apoll = apoll
        self.sock = sock
        apoll.recvfrom(sock, 65536)

    def poll(self, timeout = None):
        events = self.apoll.poll(timeout)

        for event in events:
            if event['type'] == 'recvfrom' and event['success'] == True:
                self.apoll.sendto(self.sock, event['addr'], event['data'])
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