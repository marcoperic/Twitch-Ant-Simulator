#include <iostream>
#include "client.c"
#include "client-controller.h"
#include <queue>
#include <string>
#include <thread>
using namespace std;

vector<string> commands;
vector<string> split(string s, string delimiter);
void server_Create_Poll(string active_colonies);
void getCommands(const char* str);
void debug();

// Extern C 
void relay(const char* str)
{
    cout << "Relay: " << str << endl;
    getCommands(str);
}
// Extern C

void debug()
{
    for (string a: commands)
    {
        cout << a + "\n";
    }

    commands.clear();
    cout << "Cleared command queue.\n";
}

typedef struct client_controller 
{
    vector<string> fetch()
    {
        cout << "Dispatching commands ...\n";
        // commands.pop_back();
        vector<string> retList = commands;
        commands.clear();
        cout << "Command list cleared.\n";
        return retList;
    }

    void server_Create_Poll(string active_colonies) // c_str() for str to char*
    {
        const char* temp = active_colonies.c_str();
        server_code = (char*)malloc(strlen(temp) + 1);
        strcpy(server_code, temp);
        printf("%s\n", server_code);
        cout << "Relaying create poll to server." << endl;
    }

    bool isReady()
    {
        return !commands.empty();
    }

    void start_client()
    {
        cout << "started!\n";
        thread client_connection(startClient);
        client_connection.detach(); // necessary to prevent stopping main thread. .join() is wrong.
    }  

    explicit client_controller()
    {
        start_client();
    }

}client_controller;

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

    if (tokens.size() == 0) // If we have no commands.
        return;
    
    commands = tokens;

    // if (commands.size() > 0)
    //     commands.pop_back(); // erase the last element
}