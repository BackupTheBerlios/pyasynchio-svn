import sys
import socket
import pyasynchio

# Create and bind socket
lsock = socket.socket()
lsock.bind((sys.argv[1], int(sys.argv[2])))
lsock.listen(5)

# Create apoll object
apoll = pyasynchio.apoll()
# Issue asynch accept 
apoll.accept(lsock)

while True:
    # poll results from apoll
    events = apoll.poll(0)
    for evt in events:
        # if accept succeeded
        if evt['type'] == 'accept':
            if evt['success']:
                # read data from new socket
                apoll.recv(evt['accept_socket'], 1024)
                # reissue accept
                apoll.accept(lsock)
        elif evt['type'] == 'recv':
            # if received some data and connection isnt broken
            if evt['success'] and evt['data'] != '':
                # echo it back
                apoll.send(evt['socket'], evt['data'])
                # read new
                apoll.recv(evt['socket'], 1024)
