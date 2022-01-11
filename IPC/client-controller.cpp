#include <iostream>
#include "client.c"
#include "client-controller.h"
#include <thread>
using namespace std;

void relay(const char* str)
{
    printf("%s\n", str);
}

struct client_controller 
{
    explicit client_controller()
    {
        start_client();
    }
}

// compile with g++ client-controller.cpp -lzmq -pthread
int main()
{
    cout << "started!\n";
    thread client_connection(startClient);
    client_connection.join();
    return 0;
}

void start_client()
{
    thread client_connection(startClient);
    client_connection.join();
}