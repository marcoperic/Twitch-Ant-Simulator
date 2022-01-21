#
#   Hello World server in Python
#   Binds REP socket to tcp://*:5555
#   Expects b"Hello" from client, replies with b"World"
#

import time
import zmq
import _thread

BUFFER_SIZE = 32 # Update this in C whenever modified.
UPDATE_INTERVAL = 5 # How many seconds between command transmissions

def encode(cmd_q):
    output = ""
    for i in cmd_q:
        output += ("" + str(i) + ";")

    if (len(output) < BUFFER_SIZE):
        output = output + ("-" * (BUFFER_SIZE - len(output)))

    return output

class Server:

    colors = {'red': 'r', 'green': 'g', 'blue': 'b', 'purple': 'p'}

    def __init__(self):
        self.command_queue = []
        self.command_queue.append('a1')
        self.command_queue.append('b2')

    def pushCmd(self, cmd):
        self.command_queue.append(cmd)

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
            time.sleep(UPDATE_INTERVAL)
            if (len(self.command_queue) > 0):
                print("Attempting command transfer")
                print(encode(self.command_queue))
                socket.send_string(encode(self.command_queue))
                print("Clearing command queue.")
                self.command_queue.clear()
            else:
                socket.send(b"-" * BUFFER_SIZE)
