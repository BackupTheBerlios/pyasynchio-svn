import sys
import socket
import pyasynchio

# Create and bind socket
lsock = socket.socket()
lsock.bind((sys.argv[1], int(sys.argv[2])))
lsock.listen(5)

# Create apoll object
apoll = pyasynchio.apoll()
# Issue first asynch accept through apoll object
apoll.accept(lsock)

while True:
    # poll results from apoll object
    events = apoll.poll()
    # process them one by one
    for name, success, more in events:
        # if operation was accept
        if name == 'accept':
            # moreover, if it is successful
            if success:
                # issue asynchronous read through apoll object
                apoll.recv(more['accept_socket'], 1024)
                # issue another asynch accept, so that more clients can connect
                apoll.accept(lsock)
        # if operation was recv
        elif name == 'recv':
            # if received some data and connection isnt broken (just as recv it presents empty string if client disconnected)
            if success and more['data'] != '':
                # issue asynchronous send to echo data back to client. No need to pick up results of this op
                apoll.send(more['socket'], more['data'])
                # issue new recv so we can echo more data
                apoll.recv(more['socket'], 1024)
# EOF :)