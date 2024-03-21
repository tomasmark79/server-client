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

#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include "enet\enet.h" // required enet library from: http://enet.bespin.org/ - MIT License

static const int SUCCESS = 0;
static const int FAILURE = 1;

using std::string;
using std::vector;

namespace ui
{

class LogUserInterface
{
public:
    template<typename T>
    LogUserInterface& operator<<(const T& value)
    {
        std::cout << value;
        return *this;
    };
    // Specialized method for manipulators of stream (ex. std::endl)
    typedef std::ostream& (*Manipulator)(std::ostream&);
    LogUserInterface& operator<<(Manipulator manip)
    {
        std::cout << manip; // manipulator application to standard output
        return *this;
    }
};

class LogDebugInterface
{
public:
    template<typename T>
    LogDebugInterface& operator<<(const T& value)
    {
        std::cout << value;
        return *this;
    };

    // Specialized method for manipulators of stream (ex. std::endl)
    typedef std::ostream& (*Manipulator)(std::ostream&);
    LogDebugInterface& operator<<(Manipulator manip)
    {
        std::cout << manip; // manipulator application to standard output
        return *this;
    }
};

}

class NetworkManager
{
    ui::LogUserInterface logUser;
    ui::LogDebugInterface logDebug;

    ENetHost*   server;
    ENetPeer*   peerServer;

    ENetHost*   client;
    ENetPeer*   peerClient;

    ENetAddress bindAddress;   // server
    ENetAddress remoteAddress; // client

    ENetEvent   event;

    string  bindHostName;
    int     bindPort;
    string  remoteHostName;
    int     remotePort;

    int maxClients;
    int channels;
    int amountIn;
    int amountOut;

    int totalConnectedClients;

    int outConnections;     // client

    char stream[256];

public:
    NetworkManager() :
        server(nullptr), client(nullptr), totalConnectedClients(0)
    {};
    ~NetworkManager()
    {
        if (server != nullptr)
            enet_host_destroy(server);
        if (client != nullptr)
            enet_host_destroy(client);
    };

    NetworkManager (NetworkManager& NetworkManager);
    NetworkManager operator= (NetworkManager& NetworkManager);

    int initENet()
    {
        return enet_initialize();
    };

    int initServer()
    {
        /* Bind the server to the default localhost.     */
        /* A specific host address can be specified by   */
        /* enet_address_set_host (& address, "x.x.x.x"); */
        enet_address_set_host(&bindAddress, const_cast<char*>(bindHostName.c_str()));
        bindAddress.port = bindPort;
        server = enet_host_create (&bindAddress  /* the address and port to bind the server host to */,
                                   maxClients   /* allow up to 32 clients and/or outgoing connections */,
                                   channels  /* allow up to 2 channels to be used, 0 and 1 */,
                                   amountIn  /* assume any amount of incoming bandwidth */,
                                   amountOut /* assume any amount of outgoing bandwidth */);
        if (server == nullptr)
        {
            logUser << "An error occurred while trying to create an ENet server host." << std::endl;
            return FAILURE;
        }
        else
        {
            logUser << "ENet server started sucessfuly." << std::endl;
        }
        return SUCCESS;
    }

    int initClient()
    {
        enet_address_set_host(&remoteAddress, const_cast<char*>(remoteHostName.c_str()));
        remoteAddress.port = remotePort;
        client = enet_host_create (nullptr /* create a client host */,
                                   outConnections /* only allow 1 outgoing connection */,
                                   channels /* allow up 2 channels to be used, 0 and 1 */,
                                   amountIn /* assume any amount of incoming bandwidth */,
                                   amountOut /* assume any amount of outgoing bandwidth */);
        if (client == nullptr)
        {
            logUser << "An error occurred while trying to create an ENet client host." << std::endl;
            return FAILURE;
        }
        else
        {
            logUser << "ENet client started sucessfuly." << std::endl;
        }
        return SUCCESS;
    }

    int connectionToTheServer(int timeouMilis)
    {
        peerServer = enet_host_connect (client, &remoteAddress, 2, 0);
        if (peerServer == nullptr)
        {
            logUser << "No available peers for initiating an ENet connection." << std::endl;
            return FAILURE;
        }

        if (enet_host_service (client, &event, timeouMilis) > 0 &&
                event.type == ENET_EVENT_TYPE_CONNECT)
        {
            peerServer = event.peer;
            event.peer -> data = (void*)"Server";
            logUser << "Connection to " << this->remoteHostName << ":" << this->remotePort << " succeeded." << std::endl;
        }
        else
        {
            /* Either the 5 seconds are up or a disconnect event was */
            /* received. Reset the peer in the event the 5 seconds   */
            /* had run out without any significant event.            */
            enet_peer_reset (peerServer);
            logUser << "Connection to " << this->remoteHostName << ":" << this->remotePort << " failed." << std::endl;
            return FAILURE;
        }
        return SUCCESS;
    }

    int serverIsRegisteringClient(int timeoutMilis)
    {
        /* Wait up to 10000 milliseconds for an event. */
        while (enet_host_service (server, &event, timeoutMilis) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_NONE:
                break;
            case ENET_EVENT_TYPE_CONNECT:
                this->peerClient = event.peer;
                event.peer -> data = (void*)"Client";
                sprintf ((char*)stream, "A new client from %x:%u registered.",
                         event.peer -> address.host,
                         event.peer -> address.port);
                logUser << stream << std::endl;
                totalConnectedClients++;
                return SUCCESS;
            case ENET_EVENT_TYPE_DISCONNECT:
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                break;
            }
        }
        return FAILURE;
    }

    // call
    // vector<int> bufToSend = {1,2,3,4,5,6,7,8,9,10};
    // net.sendVectorToServer(bufToSend, bufToSend.size());
    int sendVectorToServer(vector<int>& vec, size_t size)
    {
        ENetPacket * packet = enet_packet_create (vec.data(),
                              vec.size() * sizeof(int),
                              ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send (peerServer, 0, packet);
        return 0;
    }

    // call
    // vector<int> bufToReceive = {};
    // int res = net.serverHostService(bufToReceive);
    int serverHostService(vector<int>& data, int timeoutMilis)
    {
        data.clear();
        while (enet_host_service (server, &event, timeoutMilis) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_NONE:
                return 0;

            case ENET_EVENT_TYPE_CONNECT:
                return 1;

            case ENET_EVENT_TYPE_DISCONNECT:
                sprintf (static_cast<char*>(stream), "%s disconnected.\n", static_cast<char*>(event.peer -> data));
                // sprintf ((char*)stream, "%s disconnected.\n", (void*)event.peer -> data);
                logUser << stream << std::endl;
                /* Reset the peer's client information. */
                event.peer -> data = NULL;
                return 2;

            case ENET_EVENT_TYPE_RECEIVE:
                logUser << "size: " << event.packet->dataLength << std::endl;
                data.insert(data.end(),
                            reinterpret_cast<int*>(event.packet->data),
                            reinterpret_cast<int*>(event.packet->data) + event.packet->dataLength / sizeof(int));

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                return 3;
            }
        }
        return 4; // no event
    }

    // call
    // vector<int> bufToSend = {1,2,3,4,5,6,7,8,9,10};
    // net.sendVectorToServer(bufToSend, bufToSend.size());
    int sendVectorToClient(vector<int>& vec, size_t size)
    {
        ENetPacket * packet = enet_packet_create (vec.data(),
                              vec.size() * sizeof(int),
                              ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send (peerClient, 0, packet);
        return 0;
    }

    // call
    // vector<int> bufToReceive = {};
    // int res = net.clientHostService(bufToReceive);
    int clientHostService(vector<int>& data, int timeoutMilis)
    {
        data.clear();
        while (enet_host_service (client, &event, timeoutMilis) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_NONE:
                return 0;

            case ENET_EVENT_TYPE_CONNECT:
                return 1;

            case ENET_EVENT_TYPE_DISCONNECT:
                sprintf (static_cast<char*>(stream), "%s disconnected.\n", static_cast<char*>(event.peer -> data));
                logUser << stream << std::endl;
                /* Reset the peer's client information. */
                event.peer -> data = NULL;
                return 2;

            case ENET_EVENT_TYPE_RECEIVE:
                logUser << "size: " << event.packet->dataLength << std::endl;
                data.insert(data.end(),
                            reinterpret_cast<int*>(event.packet->data),
                            reinterpret_cast<int*>(event.packet->data) + event.packet->dataLength / sizeof(int));

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                return 3;
            }
        }
        return 4; // no event
    }

    int registerClients(int NetworkManagerTimeoutMilis, int timeoutTotalSeconds)
    {
        logUser << "Server" << std::endl;

        this->initServer();
        logUser << std::endl << "Waiting for a clients ..." << std::endl;

        // wait for continue if maxClients or timeout occured
        const time_t startTime = time(nullptr);
        while ( (time(nullptr) - startTime < timeoutTotalSeconds) ) // total timeout
        {
            if (this->serverIsRegisteringClient(NetworkManagerTimeoutMilis) == 0)
            {
                Beep(1500,20); // connected
                if (this->getTotalConnectedClients() == 1)
                    return 0;
            }
        }
        return 1;
    }

    int registerServer(int NetworkManagerTimeoutMilis, int timeoutTotalSeconds)
    {
        vector<int> eraryVector;
        logUser << "Client" << std::endl;
        this->initClient();
        logUser << std::endl << "Connecting to the server ..." << std::endl;

        // Client must trying connect despite server is running later that client
        // Therefore more attempts to connect every second
        const time_t startTime = time(nullptr);
        while ( (time(nullptr) - startTime < timeoutTotalSeconds) ) // total timeout
        {
            if (this->connectionToTheServer(NetworkManagerTimeoutMilis) == 0)
            {
                // neccessary - will accept connection request
                this->clientHostService(eraryVector, 50);
                Beep(1100,20); // connected
                return 0;
            }
        }
        return 1;
    }

    // ---------------------------------------------------
    string getBindHostName() const
    {
        return this->bindHostName;
    }
    NetworkManager& setBindHostName(string bindHostName)
    {
        this->bindHostName = bindHostName;
        return *this;
    }
    // ---------------------------------------------------
    string getRemoteHostName() const
    {
        return this->remoteHostName;
    }
    NetworkManager& setRemoteHostName(string remoteHostName)
    {
        this->remoteHostName = remoteHostName;
        return *this;
    }
    // ---------------------------------------------------
    int getRemotePort() const
    {
        return this->remotePort;
    }
    NetworkManager& setRemotePort(int port)
    {
        this->remotePort = port;
        return *this;
    }
    // ---------------------------------------------------
    int getBindPort() const
    {
        return this->bindPort;
    }
    NetworkManager& setBindPort(int port)
    {
        this->bindPort = port;
        return *this;
    }
    // ---------------------------------------------------
    int getMaxClients() const
    {
        return this->maxClients;
    }
    NetworkManager& setMaxClients(int maxClients)
    {
        this->maxClients = maxClients;
        return *this;
    }
    // ---------------------------------------------------
    int getChannels() const
    {
        return this->channels;
    }
    NetworkManager& setChannels(int channels)
    {
        this->channels = channels;
        return *this;
    }
    // ---------------------------------------------------
    int getAmountIn() const
    {
        return this->amountIn;
    }
    NetworkManager& setAmountIn(int amountIn)
    {
        this->amountIn = amountIn;
        return *this;
    }
    // ---------------------------------------------------
    int getAmountOut() const
    {
        return this->amountOut;
    }
    NetworkManager& setAmountOut(int amountOut)
    {
        this->amountOut = amountOut;
        return *this;
    }
    // ---------------------------------------------------
    int getOutConnections() const
    {
        return this->outConnections;
    }
    NetworkManager& setOutConnections(int outConnections)
    {
        this->outConnections = outConnections;
        return *this;
    }
    // ---------------------------------------------------
    int getTotalConnectedClients() const
    {
        return this->totalConnectedClients;
    }
    // ---------------------------------------------------
};
