#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "client-controller.h"

void startClient()
{
    printf ("Connecting to hello world server…\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
    zmq_send (requester, "ping", 4, 0);

    // avoid buffer problems by adding a terminating character at end of command sequence or use a dynamic size buffer that first takes a size param from the program.
    int request_nbr;
    while (1) {
        char buffer [256]; // buffer should match size of message otherwise wonky string things occur.
        printf ("Waiting for input ...\n");
        zmq_recv (requester, buffer, 256, 0);
        // if (zmq_recv)
        relay(buffer);
            
        zmq_send (requester, "ping", 4, 0);
        // relay(buffer);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
}