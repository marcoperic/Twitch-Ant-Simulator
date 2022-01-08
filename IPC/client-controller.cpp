#include <iostream>
#include "client.c"
#include "client-controller.h"
#include <thread>
using namespace std;

void relay(const char* str)
{
    printf("%s\n", str);
}

// compile with g++ client-controller.cpp -lzmq -pthread
int main()
{
    cout << "started!\n";
    thread client_connection(startClient);
    client_connection.join();
    return 0;
}