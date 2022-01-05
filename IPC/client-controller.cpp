#include <iostream>
#include "client.c"
#include "client-controller.h"
#include <thread>
using namespace std;

void relay(const char* str)
{
    printf("%s\n", str);
}

// -lzmq flag for compilation
int main()
{
    cout << "started!\n";
    thread client_connection(startClient);
    client_connection.join();
    return 0;
}