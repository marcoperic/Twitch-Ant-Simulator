import time
import zmq
import _thread
import random

BUFFER_SIZE = 32 # Update this in C whenever modified.
UPDATE_INTERVAL = 5 # How many seconds between command transmissions

# spawn, feed, quick (speed), kill, more ants (lower food req for spawning)
poll_codes = ['s', 'f', 'q', 'k', 'm']

def get_key(val, colors):
    for key, value in colors.items():
         if val == value:
             return str(key)
 
    return None

def encode(cmd_q):
    output = ""
    for i in cmd_q:
        output += ("" + str(i) + ";")

    if (len(output) < BUFFER_SIZE):
        output = output + ("-" * (BUFFER_SIZE - len(output)))

    return output
        
def getColonies(str, colors):
    colonies = []
    for c in str:
        if (c in colors.values()):
            colonies.append({'title' : '' + get_key(c, colors)})

    return colonies

def getRandomCode():
    return random.randint(0, len(poll_codes) - 1) # might cause index problem

class Server:

    colors = {'red': 'r', 'green': 'g', 'blue': 'b', 'cyan': 'c'}
    events = ('poison', 'candy')

    def __init__(self, chatbot):
        self.chatbot = chatbot
        self.command_queue = []
        self.poll_info = []

    def pushCmd(self, cmd):
        self.command_queue.append(cmd)
    
    def startPoll(self, code, cols):
        if (len(self.poll_info) > 0): # We already have active poll data
            print ('Poll currently active. Stopping.')
            return

        poll_data = {}
        type = code
        colonies = getColonies(cols, self.colors)
        print(colonies)
        
        if (type == 's'):
            quantity = 250 # Spawn 250 ants
            self.poll_info.append('S_' + str(quantity))
            poll_data = {'title': 'Vote for a colony to spawn ' + str(quantity) + ' ants!',
                         'choices' : colonies}

        elif (type == 'f'):
            quantity = 250
            self.poll_info.append('F_' + str(quantity))
            poll_data = {'title': 'Vote for a colony to spawn ' + str(quantity) + ' units of food!',
                         'choices' : colonies}

        elif (type == 'q'):
            modifier = 8 # Added to base speed of 40.0f
            self.poll_info.append('Q_' + str(modifier))
            poll_data = {'title': 'Vote for a colony to increase ant speed !',
                         'choices' : colonies}

        elif (type == 'k'):
            quantity = 250
            self.poll_info.append('K_' + str(quantity))
            poll_data = {'title': 'Vote for a colony to kill ' + str(quantity) + ' ants!',
                         'choices' : colonies}

        elif (type == 'm'):
            modifier = 1 # Divided by 10.0
            self.poll_info.append('M_' + str(modifier))
            poll_data = {'title': 'Vote for a colony to increase spawn rate!',
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

            if (message[0] == '_'):
                print('Attempting to start poll')
                cols = message[1:]
                self.startPoll(poll_codes[getRandomCode()], cols)
                # self.startPoll(poll_codes[4], cols)

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