#include <iostream>
#include <queue>
// #include <thread>
#include "client.c"
#include "client-controller.h"
using namespace std;

// Try implementing threading after everything is sure to work.

struct commandQueue
{
    queue<string> commands;
    // thread IPC_thread(startClient());
    startClient();

    commandQueue() = default;

    commandQueue& getCommandQueue()
    {
        return &commandQueue;
    }

    int getCommandCount()
    {
        return commands.size();
    }

    bool pushCmd(string cmd)
    {
        commands.push(cmd);
    }

    string popCmd() // POSSIBLE ERROR BY NOT DELETING FIRST ITEM. TRY POP() TOO.
    {
        return commands.front();
    }

    // thread& getIPCThread()
    // {
    //     return &IPC_thread;
    // }
}

