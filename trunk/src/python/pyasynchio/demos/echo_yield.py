import pyasynchio
import socket

lsock = socket.socket()
lsock.bind(('', 3811))
lsock.listen(5);

ap = pyasynchio.apoll();

class echo_handler:
    def __init__(self, sock):
        self.genobj = self(sock)
    def __call__(self, sock):
        ap.recv(sock, 1024, act = self);
        yield None
        while True:
            if self.spec.data == "":
                return;
            ap.send(sock, self.spec.data, act = self);
            yield None
            ap.recv(sock, 1024, act = self);
            yield None

    def next(self):
        self.genobj.next()

    def notify(self, gen, spec):
        self.gen = gen
        self.spec = spec

class accept_handler:
    def __init__(self, lsock):
        self.genobj = self(lsock)

    def __call__(self, sock):
        while True:
            print lsock
            ap.accept(lsock, act = self)
            yield None   
            print 'accepted' 
            eh = echo_handler(self.spec.asock)
            eh.notify(self.gen, self.spec)
            eh.next()

    def next(self):
        self.genobj.next();
        
    def notify(self, gen, spec):
        self.gen = gen
        self.spec = spec

ah = accept_handler(lsock)
ah.next();

while True:
    results = ap.poll();
    for gen, spec in results:
        gen.act.notify(gen, spec);
        try:
            gen.act.next()
        except StopIteration:
            continue;
    
    
    