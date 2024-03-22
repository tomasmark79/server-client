//MIT License
//
//Copyright (c) 2024 Tomas Mark
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include <iostream>
#include <winsock2.h>
#include "NetworkManager.h"
#include <stdexcept>
#include <ctime>
#include <windows.h>

using namespace std;

int main(int argc, char *argv[], char* env[])
{
    bool isServer = false;
    try
    {
        if (argc == 2)
        {
            if (argv[1][0] == 's')
                isServer = true;
        }
    }
    catch(exception &e) {};

    // // // // // // // // // // // // // // // // // // // // //
    NetworkManager net;
    vector<int> eraryVector;
    // // // // // // // // // // // // // // // // // // // // //

    // Connection each other
    if (isServer)
    {
        // Server Session
        std::cout << "Server session started ... " << std::endl;
        net
        .setIsServer(true)
        .setBindHostName("192.168.79.101")
        .setBindPort(7996)
        .setMaxClients(4)
        .setChannels(2)
        .setAmountIn(0)
        .setAmountOut(0);
        net.initENet();
        net.registerClients(1000, 10);
    }
    else
    {
        // Client Session
        std::cout << "Client session started ... " << std::endl;
        net
        .setIsServer(false)
        .setRemoteHostName("192.168.79.101")
        .setRemotePort(7996)
        .setOutConnections(1);
        net.initENet();
        net.registerServer(1000, 10);
    }

    // bidirectional communication transfer data there and back in vector<int> type
    vector<int> bufToSendtoServer = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    vector<int> bufToSendtoClient = {15,16,17,18,999,65535};

    //
    //
    // example of main transfer loop
    //
    //
#define DATA_RECEIVED   (int)3

    time_t startTime = time(nullptr);
    while ( (time(nullptr) - startTime < 20 /*main timeout for tests*/) )
    {
        if (isServer)
        {
            // receiving data from other side
            int status = net.serverHostService(eraryVector, 50);
            if (eraryVector.size() > 0 && status == DATA_RECEIVED)
            {
                for (size_t i = 0; i < eraryVector.size(); i++)
                    cout << eraryVector[i] << " ";
                cout << endl;
            }

            // Send vector to client
            net.sendVectorToClient(bufToSendtoClient, bufToSendtoClient.size());
        }
        else
        {
            // receiving data from other side
            int status = net.clientHostService(eraryVector, 50);
            if (eraryVector.size() > 0 && status == DATA_RECEIVED)
            {
                for (size_t i = 0; i < eraryVector.size(); i++)
                    cout << eraryVector[i] << " ";
                cout << endl;
            }
            // Send vector to server
            net.sendVectorToServer(bufToSendtoServer, bufToSendtoServer.size());
        }
    }







    return 0;
}
