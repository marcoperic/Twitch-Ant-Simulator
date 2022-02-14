import zmq
context = zmq.Context()

print("Connecting to command dispatch ...")
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5454")

while True:
    cmd = input('enter command')
    socket.send(cmd.encode())
    reply = socket.recv()
