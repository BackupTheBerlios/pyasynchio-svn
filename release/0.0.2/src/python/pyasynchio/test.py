import unittest
import pyasynchio
import socket
from echo import Echo

class EchoFixture(unittest.TestCase):
    
    def setUp(self):
        self.pro = pyasynchio.Proactor()
        self.port = 40274
        self.echo = Echo(self.pro)
        self.done = False
        self.pro.open_stream_accept(self.echo, ('', self.port))
        import thread
        thread.start_new_thread(self.thr_func, ())
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(('127.0.0.1', self.port))

    def thr_func(self):
        while not self.done:
            self.pro.handle_events(0.001)

    def tearDown(self):
        self.done = True
        self.sock.send("shutdown")
        self.sock.close()

class TestEcho(EchoFixture):
    def test_it(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(('127.0.0.1', self.port))
        data = "data for sending"
        self.assert_(sock.send(data) == len(data))
        self.assert_(sock.recv(len(data)) == data)
        self.echo.close()

        biggerstuff = "stuffy" * 100000

        echoed = ''
        max_portion_size = 204800
        while(len(echoed) != len(biggerstuff)):
            portion_size = min(max_portion_size, len(biggerstuff) - len(echoed))
            portion = biggerstuff[len(echoed):len(echoed) + portion_size]
            sock.send(portion)
            recvbuf = ''
            while(len(recvbuf) != len(portion)):
                recvbuf += sock.recv(len(portion))
            echoed += recvbuf

        self.assert_(echoed == biggerstuff)

class TestTimerSignal(unittest.TestCase, pyasynchio.AbstractTimerHandler):
    def setUp(self):
        pyasynchio.AbstractTimerHandler.__init__(self)
        self.pro = pyasynchio.Proactor()
        self.done = False
        import threading
        self.log_event = threading.Event()
        import thread
        thread.start_new_thread(self.thr_func, ())

    def tearDown(self):
        self.done = True                

    def thr_func(self):
        while not self.done:
            self.pro.handle_events(0.001)

    def test_simple(self):
        self.log = []
        import time
        abstime = time.time()
        delay800ms = 0.8
        self.pro.schedule_timer(delay800ms, self)
        delay500ms = 0.5
        self.pro.schedule_timer(delay500ms, self)
        self.log_event.wait(1)
        self.log_event.clear()
        self.assert_(len(self.log) >= 1)
        self.assert_(self.log[-1] >= abstime + 0.5)
        self.log_event.wait(1)
        self.log_event.clear()
        self.assert_(len(self.log) == 2)
        self.assert_(self.log[-1] >= abstime + 0.8)

    def notify_elapsed(self, tv):
        import time
        self.log.append(time.time())
        self.log_event.set()
        

suite = unittest.TestSuite()
suite.addTest(unittest.makeSuite(TestEcho))
suite.addTest(unittest.makeSuite(TestTimerSignal))

def run():
    unittest.TextTestRunner().run(suite)