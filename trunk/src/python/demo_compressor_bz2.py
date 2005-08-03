from bz2 import BZ2Compressor
import socket
import thread


def main(argv):
    if argv[1] == 'asynch_server':
        class asynch_bz2_server:
            chunk_size = 17520
            
            class bz2_stream:
                def __init__(self, pro):
                    self.__pro = pro
                    self.__compressor = BZ2Compressor()
                    self.__dying = False
                    self.__size = 0
                    self.__processed = 0

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
                self.__pro = pro

            def close(self):
                self.pro.close_stream_accept(self)

            def make_stream_handler(self):
                return self.bz2_stream(self.__pro)

        from pyasynchio import Proactor
        pro = Proactor()
        port = int(argv[2])
        bz2_srv = asynch_bz2_server(pro)
        pro.open_stream_accept(bz2_srv, ('', port))
    
        pro.handle_events_forever()
            

    if argv[1] == 'synch_server':
        try:
            from select import poll, error, POLLIN, POLLOUT
        except ImportError:
            from selectpoll import poll, error, POLLIN, POLLOUT
        acceptor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        port = int(argv[2])
        acceptor.bind(('', port))
        acceptor.setblocking(0)
        acceptor.listen(5)
        acceptor_fd = acceptor.fileno()
        poller = poll()
        poller.register(acceptor_fd, POLLIN)
        CONN_READING = 0
        CONN_WRITING = 1
        CHUNK_SIZE = 17520
        conn_states = {}

        while True:
            events = poller.poll()
            for event in events:
                if event == (acceptor_fd, POLLIN):
                    # new connection
                    sock = acceptor.accept()[0]
                    sock.setblocking(0)
                    conn_states[sock.fileno()] = { 'sock' : sock
                                    , 'size' : 0
                                    , 'processed' : 0
                                    , 'to_write' : ''   
                                    , 'written' : 0
                                    , 'compressor' : BZ2Compressor() }
                    poller.register(sock.fileno(), POLLIN | POLLOUT)
                elif event[1] == POLLIN:
                    conn_state = conn_states[event[0]]
                    sock = conn_state['sock']
                    data = sock.recv(CHUNK_SIZE)
                    if conn_state['size'] == 0:
                        data_size = int(data[:data.find('x')])
                        data = data[data.find('x') + 1:]
                        conn_state['size'] = data_size
                    conn_state['processed'] += len(data)
                    conn_state['to_write'] += conn_state['compressor'].compress(data)
                    if conn_state['processed'] == conn_state['size']:
                        conn_state['to_write'] += conn_state['compressor'].flush()
                elif event[1] == POLLOUT:
                    conn_state = conn_states[event[0]]
                    if conn_state['to_write'] == '':
                        continue
                    sock = conn_state['sock']
                    cb = sock.send(conn_state['to_write'][conn_state['written']:])
                    conn_state['written'] += cb
                    if conn_state['written'] == len(conn_state['to_write']):
                        del conn_states[event[0]]
                        poller.unregister(event[0])
                        sock.close()                    
    if argv[1] == 'client':
        server_address = argv[2]
        server_port = int(argv[3])
        from random import choice, randrange
        from bz2 import BZ2Decompressor
        from time import time
        start = time()
        total = 0
        while True:
#            decompressor = BZ2Decompressor()
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.connect((server_address, server_port))
            data = choice(['a;dlfj', '371047', '238194', 'sghbalfd', '*()17e'])
            data *= randrange(90000, 100000)
            sock.sendall(str(len(data)) + 'x')
            sock.sendall(data)

            data_cc = sock.recv(len(data))
            data_c = data_cc
            while len(data_cc):
                data_cc = sock.recv(len(data))
                data_c += data_cc
            
#            assert(data == decompressor.decompress(data_c))
            sock.close()
            total += len(data)
            if time() != start:
                print 'rate is %g b/s' % (total / (time() - start))

if __name__ == '__main__':
    from sys import argv
    main(argv)


"""
        acceptor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        port = int(argv[2])
        acceptor.bind(('', port))
        acceptor.listen(5)
        while True:
            sock = acceptor.accept()[0]

            def thr_func(sock):
                chunk_size = 17520
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

            thread.start_new_thread(thr_func, (sock,))
"""
                    