#include <iostream>
#include "client.c"
using namespace std;

// -lzmq flag for compilation
int main()
{
    cout << "started!\n";
    startClient();
    return 0;
}