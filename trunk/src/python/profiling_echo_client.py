import socket
import sys
import random
import time

server_addr = sys.argv[1]
server_port = int(sys.argv[2])
total_bytes = 0
start = time.time()
last = start
while True:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((server_addr, server_port))
    msg = 'to be echoed' * 10
    sock.sendall(msg)
    echoed = ''
    while len(echoed) < len(msg):
        echoed += sock.recv(102400)
#    echoed = sock.recv(len(msg))
    assert(echoed == msg)
    sock.sendall('shutdown')
    sock.close()
    total_bytes += len(msg)
    ct = time.time()
    if ct - last != 0 and ct > (last + 1.0) :
        print 'rate is %g b/s' % (total_bytes / (ct - last))
        last = ct
        total_bytes = 0