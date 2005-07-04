import unittest
import pyasynchio
import socket
from echo import Echo

class EchoFixture(unittest.TestCase):
    
    def setUp(self):
        self.pro = pyasynchio.Proactor(False)
        self.port = 40274
        self.ac = pyasynchio.AcceptContext()
        self.echo = Echo(self.pro, self.ac)
        self.done = False
        self.pro.accept(self.ac, ('', self.port))
        import thread
        thread.start_new_thread(self.thr_func, ())
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(('127.0.0.1', self.port))

    def thr_func(self):
        while not self.done:
            self.pro.handleEvents((0,1000))

    def tearDown(self):
        self.done = True
        self.sock.send("shutdown")
        self.sock.close()

class TestEcho(EchoFixture):
    def test_(self):
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

class TestTimerSignal(unittest.TestCase):
    def setUp(self):
        self.pro = pyasynchio.Proactor(False)
        self.done = False
        import threading
        self.log_event = threading.Event()
        import thread
        thread.start_new_thread(self.thr_func, ())

    def tearDown(self):
        self.done = True                

    def thr_func(self):
        while not self.done:
            self.pro.handleEvents((0,1000))

    def test_simple(self):
        first = pyasynchio.TimerSignal()
        second = pyasynchio.TimerSignal()
        self.log = []
        first.connect(self.onTimer)
        second.connect(self.onTimer)
        import time
        abstime = time.time()
        delay800ms = (0, 800000)
        self.pro.scheduleTimer(delay800ms, first)
        delay500ms = (0, 500000)
        self.pro.scheduleTimer(delay500ms, second)
        self.log_event.wait(1)
        self.log_event.clear()
        self.assert_(len(self.log) >= 1)
        self.assert_(self.log[-1] >= abstime + 0.5)
        self.log_event.wait(1)
        self.log_event.clear()
        self.assert_(len(self.log) == 2)
        self.assert_(self.log[-1] >= abstime + 0.8)

    def onTimer(self, tv):
        import time
        self.log.append(time.time())
        self.log_event.set()
        

suite = unittest.TestSuite()
suite.addTest(unittest.makeSuite(TestEcho))
suite.addTest(unittest.makeSuite(TestTimerSignal))

def run():
    unittest.TextTestRunner().run(suite)