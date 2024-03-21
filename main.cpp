#include <iostream>
#include <winsock2.h>
#include "Network.h"
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
    Network net;
    vector<int> eraryVector;

    // server
    net
    .setBindHostName("192.168.79.101")
    .setBindPort(7995)
    .setMaxClients(4)
    .setChannels(2)
    .setAmountIn(0)
    .setAmountOut(0);

    // client
    net
    .setRemoteHostName("192.168.79.101")
    .setRemotePort(7995)
    .setOutConnections(1)
    .initENet();

    // Connection each other
    if (isServer)
    {
        net.registerClients(1000, 10);
    }
    else
    {
        net.registerServer(1000, 10);
    }

    // bidirectional communication transfer data there and back in vector<int> type
    vector<int> bufToSendtoServer = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    vector<int> bufToSendtoClient = {15,16,17,18,999,65535};
    time_t startTime = time(nullptr);
    while ( (time(nullptr) - startTime < 20) )
    {
        if (isServer)
        {
            int status = net.serverHostService(eraryVector, 50);
            if (eraryVector.size() > 0 && status == 3)
            {
                for (size_t i = 0; i < eraryVector.size(); i++)
                    cout << eraryVector[i] << " ";
                cout << endl;
            }

            net.sendVectorToClient(bufToSendtoClient, bufToSendtoClient.size());
        }
        else
        {
            int status = net.clientHostService(eraryVector, 50);
            if (eraryVector.size() > 0 && status == 3)
            {
                for (size_t i = 0; i < eraryVector.size(); i++)
                    cout << eraryVector[i] << " ";
                cout << endl;
            }

            net.sendVectorToServer(bufToSendtoServer, bufToSendtoServer.size());
        }
    }







    return 0;
}
