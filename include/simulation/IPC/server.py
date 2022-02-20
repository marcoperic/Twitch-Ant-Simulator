import time
import zmq
import _thread
import random

BUFFER_SIZE = 32 # Update this in C whenever modified.
UPDATE_INTERVAL = 5 # How many seconds between command transmissions

# spawn, feed, quick (speed), kill, more ants (lower food req for spawning)
poll_codes = ['s', 'f', 'q', 'k', 'm']

def encode(cmd_q):
    output = ""
    for i in cmd_q:
        output += ("" + str(i) + ";")

    if (len(output) < BUFFER_SIZE):
        output = output + ("-" * (BUFFER_SIZE - len(output)))

    return output
        
def getColonies(str):
    colonies = []
    for c in str:
        if (c in colors.values()):
            colonies.append({'title' : str(colors.index(c))})

def getRandomCode():
    random.randint(0, len(poll_codes) - 1) # might cause index problem

class Server:

    colors = {'red': 'r', 'green': 'g', 'blue': 'b', 'cyan': 'c'}
    events = ('poison', 'candy')

    def __init__(self, chatbot):
        self.chatbot = chatbot
        self.command_queue = []
        self.poll_info = []
        # self.command_queue.append('test1')
        # self.command_queue.append('test2')

    def pushCmd(self, cmd):
        self.command_queue.append(cmd)
    
    # Code should show which colonies are eligible to receive the modifier.
    # Try format like: s100_rgbc For spawn 100 for the following colonies
    def startPoll(self, code):
        if (len(self.poll_info) > 0): # We already have active poll data
            return

        poll_data = {}
        tokens = code.split('_')
        type = tokens[0][0]
        colonies = getColonies(tokens[1])
    
        if (type == 's'):
            quantity = int(tokens[0][1:])
            self.poll_info.append('spawn_' + quantity)
            poll_data = {'title': 'Vote for a colony to spawn ' + quantity + ' ants!',
                         'choices' : colonies}

        self.chatbot.create_poll(poll_data)

    def init_threading(self):
        print('Starting threads.')
        _thread.start_new_thread(self.init_connections, ()) # Client control
        _thread.start_new_thread(self.admin_console, ()) # Admin commands
    
    def admin_console(self):
        context = zmq.Context()
        socket = context.socket(zmq.REP)
        socket.bind("tcp://*:5454") # Different port

        print('Listening for incoming requests')
        while True:
            #  Wait for next request from client
            message = socket.recv()
            print("Admin transmission: %s" % message)
            self.command_queue.append("@" + message.decode('utf-8')) # decode from bytes to str
            socket.send(b"reply")

    def init_connections(self):
        context = zmq.Context()
        socket = context.socket(zmq.REP)
        socket.bind("tcp://*:5555")

        print('Listening for incoming requests')
        while True:
            #  Wait for next request from client
            message = socket.recv()
            print("Received request: %s" % message)
            message = message.decode('UTF-8')
            print(message)

            if (message == '1'):
                print('Attempting to start poll')
                # startPoll(poll_codes[getRandomCode()])
                self.startPoll(poll_codes[0])

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