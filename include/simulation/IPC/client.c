#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "client-controller.h"
#define BUFFER_SIZE 32 // Be sure to update in Python class.

char* server_code = NULL;

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

        if (server_code != NULL)
        {
            zmq_send (requester, server_code, strlen(server_code), 0); //sizeof cause problems
        }
        else
        {
            zmq_send(requester, "ping", 4, 0);
        }
        // relay(buffer);

        free(server_code);
        server_code = NULL;
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
}