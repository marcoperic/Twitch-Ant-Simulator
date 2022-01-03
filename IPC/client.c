//  Hello World client
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "client-controller.h"

// int main (void)
// {
//     printf ("Connecting to hello world server…\n");
//     void *context = zmq_ctx_new ();
//     void *requester = zmq_socket (context, ZMQ_REQ);
//     zmq_connect (requester, "tcp://localhost:5555");

//     int request_nbr;
//     for (request_nbr = 0; request_nbr != 10; request_nbr++) {
//         char buffer [10];
//         printf ("Sending Hello %d…\n", request_nbr);
//         zmq_send (requester, "Hello", 5, 0);
//         zmq_recv (requester, buffer, 10, 0);
//         printf ("Received World %d\n", request_nbr);
//     }
//     zmq_close (requester);
//     zmq_ctx_destroy (context);
//     return 0;
// }

void startClient()
{
    printf ("Connecting to hello world server…\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
    // while (strcmp(buffer, "flg_terminate0") != 0)
    // {
    //     char buffer = [64]; // subject to change
    //     printf("Waiting for input");
    //     zmq_recv(requester, buffer, 64, 0);
    //     printf("")
    // }

    // zmq_close(requester);
    // zmq_ctx_destroy(context);

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        printf ("Sending Hello %d…\n", request_nbr);
        zmq_send (requester, "Hello", 5, 0);
        zmq_recv (requester, buffer, 10, 0);
        printf ("Received World %d\n", request_nbr);
        relay(buffer);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
}