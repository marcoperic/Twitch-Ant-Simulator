#
#   Hello World server in Python
#   Binds REP socket to tcp://*:5555
#   Expects b"Hello" from client, replies with b"World"
#

import time
import zmq
import _thread

def encode(cmd_q):
    output = ""
    for i in cmd_q:
        output += ("" + str(i) + ";")

    return output

class Server:

    def __init__(self):
        self.command_queue = []
        self.command_queue.append('a1')
        self.command_queue.append('b2')


    def init_threading(self):
        print('Starting threads.')
        _thread.start_new_thread(self.init_connections, ())

    def init_connections(self):
        self.test = 2
        context = zmq.Context()
        socket = context.socket(zmq.REP)
        socket.bind("tcp://*:5555")

        print('Listening for incoming requests')
        while True:
            #  Wait for next request from client
            message = socket.recv()
            print("Received request: %s" % message)

            # Check to see if object has been populated with commands from user. If greater than one, send commands down pipeline.
            time.sleep(5)
            if (len(self.command_queue) > 0):
                print("Attempting command transfer")
                print(encode(self.command_queue))
                socket.send_string(encode(self.command_queue))
            else:
                socket.send(b"No commands to send.")

# Uncomment and run to test IPC between C and C++ interfaces.
# context = zmq.Context()
# socket = context.socket(zmq.REP)
# socket.bind("tcp://*:5555")
# # connected = socket.recv() # awaiting connection

# print('Listening for incoming requests')
# while True:
#     #  Wait for next request from client
#     message = socket.recv()
#     print("Received request")

#     time.sleep(1) # may set to 10 or 15s
#     # Check to see if object has been populated with commands from user. If greater than one, send commands down pipeline.

#     #  Send reply back to client
#     socket.send(b"12345678")
