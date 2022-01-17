#include <iostream>
#include "client.c"
#include "client-controller.h"
#include <thread>
using namespace std;

// Extern C 
void relay(const char* str)
{
    printf("%s\n", str);
}

typedef struct client_controller 
{
    void start_client()
    {
        cout << "started!\n";
        thread client_connection(startClient);
        client_connection.join();
    }  

    explicit client_controller()
    {
        start_client();
    }

}client_controller;

// compile with g++ client-controller.cpp -lzmq -pthread
int main()
{
    client_controller c;
    // start_client();
    return 0;
}