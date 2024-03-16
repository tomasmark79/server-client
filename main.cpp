#include <iostream>
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
    net.setBindHostName("192.168.79.101");
    net.setBindPort(7995);
    net.setMaxClients(4);
    net.setChannels(2);
    net.setAmountIn(0);
    net.setAmountOut(0);

    // client
    net.setRemoteHostName("192.168.79.101");
    net.setRemotePort(7995);
    net.setOutConnections(1);
    net.initENet();

    // Connection each other
    if (isServer)
    {
        cout << "Server" << endl;
        net.initServer();
        cout << endl << "Waiting for a clients ..." << endl;

        // wait for continue if maxClients or timeout occured
        const time_t startTime = time(nullptr);
        while ( (time(nullptr) - startTime < 10) )
        {
            if (net.serverIsRegisteringClient() == 0)
            {
                Beep(1500,20); // connected
                if (net.getTotalConnectedClients() == 1)
                    break;
            }
        }
    }
    else
    {
        cout << "Client" << endl;
        net.initClient();
        cout << endl << "Connecting to the server ..." << endl;
        if (net.connectionToTheServer() == 0)
        {
            // neccessary = important - is managing bidirectional transfer
            net.clientHostService(eraryVector);
            // Here we can handle more client transfers
            // Better will be in different condition below
        }
    }

    // transfer data there and back in vector<int> type
    vector<int> bufToSendtoServer = {1,2};
    vector<int> bufToSendtoClient = {3,4};

    time_t startTime = time(nullptr);
    while ( (time(nullptr) - startTime < 10) )
    {
        if (isServer)
        {
            int res = net.serverHostService(eraryVector);
            if (eraryVector.size() > 0)
            {
                for (int i = 0; i < eraryVector.size(); i++)
                    cout << eraryVector[i] << " ";
                cout << endl;
            }

            net.sendVectorToClient(bufToSendtoClient, bufToSendtoClient.size());
        }
        else
        {
            int res = net.clientHostService(eraryVector);
            if (eraryVector.size() > 0)
            {
                for (int i = 0; i < eraryVector.size(); i++)
                    cout << eraryVector[i] << " ";
                cout << endl;
            }

            net.sendVectorToServer(bufToSendtoServer, bufToSendtoServer.size());
        }
    }



    return 0;
}