#include <iostream>
#include "client.c"
#include "client-controller.h"
#include <queue>
#include <string>
#include <thread>
using namespace std;

vector<string> commands;
vector<string> split(string s, string delimiter);
void getCommands(const char* str);
void dispatchCommands();

// Extern C 
void relay(const char* str)
{
    printf("%s\n", str);
    getCommands(str);
    dispatchCommands();
}
// Extern C

typedef struct client_controller 
{
    int getCommandCount()
    {
        return commands.size();
    }

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

void dispatchCommands()
{
    cout << "Dispatching commands ...\n";
    // Print out the commands
    // for (string a: commands)
    //     cout << a + "\n";
}

vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void getCommands(const char* str)
{
    string temp(str);
    vector<string> tokens = split(temp, ";");  
    commands = tokens;
}


// compile with g++ client-controller.cpp -lzmq -pthread
int main()
{
    client_controller c;
    // start_client();
    return 0;
}