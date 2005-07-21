from pyasynchio import AbstractAcceptHandler, AbstractStreamHandler, Proactor
from bz2 import BZ2Compressor

class asynch_bz2_server(AbstractAcceptHandler):
    chunk_size = 17520
    
    class bz2_stream(AbstractStreamHandler):
        def __init__(self, pro):
            AbstractStreamHandler.__init__(self)
            self.__pro = pro
            self.__compressor = BZ2Compressor()
            self.__dying = False
            self.__size = 0
            self.__processed = 0

        def notify_endpoints(self, remote, local): 
            pass

        def notify_initialized(self):
            self.__pro.open_stream_read(self, asynch_bz2_server.chunk_size)

        def read_completed(self, read_result):
            if (not read_result.success_):
                return
            data = read_result.data_
            if self.__size == 0:
                self.__size = int(data[:data.find('x')])
                data = data[data.find('x')+1:]

            self.__processed += len(data)
            data_c = self.__compressor.compress(data)
            if data_c != '':
                self.__pro.open_stream_write(self, data_c)
            if self.__processed == self.__size:
                data_c = self.__compressor.flush()    
                self.__dying = True
                self.__pro.open_stream_write(self, data_c)
                return
             
            self.__pro.open_stream_read(self, 
                min(asynch_bz2_server.chunk_size, self.__size - self.__processed))
                
                

        def write_completed(self, write_result):
            if self.__dying:
                self.__pro.close_active_stream(self)

    def __init__(self, pro):
        AbstractAcceptHandler.__init__(self)
        self.__pro = pro

    def close(self):
        self.pro.close_stream_accept(self)

    def conn_validate(self, result, remote, local):
        return True

    def conn_completed(self, result):
        pass

    def make_stream_handler(self):
        return self.bz2_stream(self.__pro)

import socket

def main(argv):
    if argv[1] == 'asynch_server':
        pro = Proactor()
        port = int(argv[2])
        bz2_srv = asynch_bz2_server(pro)
        pro.open_stream_accept(bz2_srv, ('', port))
        while True:
            pro.handle_events(0.001)
            

    if argv[1] == 'synch_server':
        acceptor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        port = int(argv[2])
        acceptor.bind(('', port))
        acceptor.listen(5)
        while True:
            sock = acceptor.accept()[0]
            chunk_size = 4096
            data = sock.recv(chunk_size)
            data_size = int(data[:data.find('x')])
            data = data[data.find('x') + 1:]
            processed = 0
            compressor = BZ2Compressor()
            while processed != data_size:
                processed += len(data)
                data_c = compressor.compress(data)
                if processed == data_size:
                    break
                if data_c != '':
                    sock.sendall(data_c)
                data = sock.recv(chunk_size)
            data_c = compressor.flush()
            sock.sendall(data_c)
            sock.close()
                

    if argv[1] == 'client':
        server_address = argv[2]
        server_port = int(argv[3])
        from random import choice, randrange
        from bz2 import BZ2Decompressor
        from time import time
        total = 0
        start = time()
        while True:
            decompressor = BZ2Decompressor()
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.connect((server_address, server_port))
            data = choice(['a;dlfj', '371047', '238194', 'sghbalfd', '*()17e'])
            data *= randrange(10000, 20000)
            sock.sendall(str(len(data)) + 'x')
            sock.sendall(data)

            data_cc = sock.recv(len(data))
            data_c = data_cc
            while len(data_cc):
                data_cc = sock.recv(len(data))
                data_c += data_cc
            
            assert(data == decompressor.decompress(data_c))
            sock.close()
            total += len(data)
            if time() != start:
                print 'rate is %g b/s' % (total / (time() - start))

if __name__ == '__main__':
    from sys import argv
    main(argv)
                    