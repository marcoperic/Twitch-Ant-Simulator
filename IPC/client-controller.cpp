#include <iostream>
#include "client.c"
#include "client-controller.h"
using namespace std;

void relay(const char* str)
{
    cout << "RELAY: ";
    cout << str;
    cout << "\n";
}

// -lzmq flag for compilation
int main()
{
    cout << "started!\n";
    startClient();
    return 0;
}