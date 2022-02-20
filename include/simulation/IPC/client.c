#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "client-controller.h"
#define BUFFER_SIZE 32 // Be sure to update in Python class.

// 1 = start poll
int code = 0;

void startClient()
{
    printf ("Connecting to Twitch IRC server\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
    zmq_send (requester, "ping", 4, 0);

    // avoid buffer problems by adding a terminating character at end of command sequence or use a dynamic size buffer that first takes a size param from the program.
    int request_nbr;
    while (1) {
        char buffer [BUFFER_SIZE]; // buffer should match size of message otherwise wonky string things occur.
        printf ("Waiting for input ...\n");
        zmq_recv (requester, buffer, BUFFER_SIZE, 0);
        // if (zmq_recv)
        relay(buffer);

        char *s = malloc(sizeof(char) + 1);
        strcpy(s, itoa(code));
        zmq_send (requester, s, 4, 0);
        // relay(buffer);

        code = 0;
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
}