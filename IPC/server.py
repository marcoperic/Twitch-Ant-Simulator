#
#   Hello World server in Python
#   Binds REP socket to tcp://*:5555
#   Expects b"Hello" from client, replies with b"World"
#

import time
import zmq
import _thread

class Server:

    def init_threading(self):
        print('Starting threads.')
        _thread.start_new_thread(self.init_connections, ())

    def init_connections(self):
        context = zmq.Context()
        socket = context.socket(zmq.REP)
        socket.bind("tcp://*:5555")

        print('Listening for incoming requests')
        while True:
            #  Wait for next request from client
            message = socket.recv()
            print("Received request: %s" % message)

            #  Do some 'work'
            time.sleep(1)

            #  Send reply back to client
            socket.send(b"World")

# Uncomment and run to test IPC between C and C++ interfaces.

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:5555")
# connected = socket.recv() # awaiting connection

print('Listening for incoming requests')
while True:
    #  Wait for next request from client
    message = socket.recv()
    print("Received request")

    time.sleep(1) # may set to 10 or 15s
    # Check to see if object has been populated with commands from user. If greater than one, send commands down pipeline.

    #  Send reply back to client
    socket.send(b"12345678")
