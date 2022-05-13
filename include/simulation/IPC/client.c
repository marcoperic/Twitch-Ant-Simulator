#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "client-controller.h"
#define BUFFER_SIZE 257 // Be sure to update in Python class.
// Approximately 64 commands can fit in this buffer.

char* server_code = NULL;

void startClient()
{
    printf ("Connecting to Twitch IRC server\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
    zmq_send (requester, "ping", 4, 0);

    while (1) {
        char buffer [BUFFER_SIZE]; // buffer should match size of message otherwise wonky string things occur.
        printf ("Waiting for input ...\n");
        zmq_recv (requester, buffer, BUFFER_SIZE, 0);
        relay(buffer);

        if (server_code != NULL)
        {
            zmq_send(requester, server_code, strlen(server_code), 0);
        }
        else
        {
            zmq_send(requester, "ping", 4, 0);
        }

        free(server_code);
        server_code = NULL;
    }

    zmq_close (requester);
    zmq_ctx_destroy (context);
}