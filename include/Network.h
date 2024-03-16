#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include "enet\enet.h"

static const int SUCCESS = 0;
static const int FAILURE = 1;

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

class Network
{
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
    Network() :
        server(nullptr), client(nullptr), totalConnectedClients(0)
    {};
    ~Network()
    {
        if (server != nullptr)
            enet_host_destroy(server);
        if (client != nullptr)
            enet_host_destroy(client);
    };

    Network (Network& network);
    Network operator= (Network& network);

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
            cerr << "An error occurred while trying to create an ENet server host." << endl;
            return FAILURE;
        }
        else
        {
            cout << "ENet server started sucessfuly." << endl;
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
            cerr << "An error occurred while trying to create an ENet client host." << endl;
            return FAILURE;
        }
        else
        {
            cout << "ENet client started sucessfuly." << endl;
        }
        return SUCCESS;
    }

    int connectionToTheServer()
    {
        peerServer = enet_host_connect (client, &remoteAddress, 2, 0);
        if (peerServer == nullptr)
        {
            cerr << "No available peers for initiating an ENet connection." << endl;
            return FAILURE;
        }

        if (enet_host_service (client, &event, 10000) > 0 &&
                event.type == ENET_EVENT_TYPE_CONNECT)
        {
            peerServer = event.peer;
            event.peer -> data = (void*)"Server";
            cout << "Connection to " << this->remoteHostName << ":" << this->remotePort << " succeeded." << endl;
        }
        else
        {
            /* Either the 5 seconds are up or a disconnect event was */
            /* received. Reset the peer in the event the 5 seconds   */
            /* had run out without any significant event.            */
            enet_peer_reset (peerServer);
            cerr << "Connection to " << this->remoteHostName << ":" << this->remotePort << " failed." << endl;
            return FAILURE;
        }
        return SUCCESS;
    }

    int serverIsRegisteringClient()
    {
        /* Wait up to 10000 milliseconds for an event. */
        while (enet_host_service (server, &event, 10000) > 0)
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
                cout << stream << endl;
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
    int serverHostService(vector<int>& data)
    {
        data.clear();
        while (enet_host_service (server, &event, 50) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_NONE:
                return 0;

            case ENET_EVENT_TYPE_CONNECT:
                return 1;

            case ENET_EVENT_TYPE_DISCONNECT:
                sprintf ((char*)stream, "%s disconnected.\n", (void*)event.peer -> data);
                cout << stream << endl;
                /* Reset the peer's client information. */
                event.peer -> data = NULL;
                return 2;

            case ENET_EVENT_TYPE_RECEIVE:
                cout << "size: " << event.packet->dataLength << endl;
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
    int clientHostService(vector<int>& data)
    {
        data.clear();
        while (enet_host_service (client, &event, 50) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_NONE:
                return 0;

            case ENET_EVENT_TYPE_CONNECT:
                return 1;

            case ENET_EVENT_TYPE_DISCONNECT:
                sprintf ((char*)stream, "%s disconnected.\n", (void*)event.peer -> data);
                cout << stream << endl;
                /* Reset the peer's client information. */
                event.peer -> data = NULL;
                return 2;

            case ENET_EVENT_TYPE_RECEIVE:
                cout << "size: " << event.packet->dataLength << endl;
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

    // ---------------------------------------------------
    string getBindHostName() const
    {
        return this->bindHostName;
    }
    void setBindHostName(string bindHostName)
    {
        this->bindHostName = bindHostName;
    }
    // ---------------------------------------------------
    string getRemoteHostName() const
    {
        return this->remoteHostName;
    }
    void setRemoteHostName(string remoteHostName)
    {
        this->remoteHostName = remoteHostName;
    }
    // ---------------------------------------------------
    int getRemotePort() const
    {
        return this->remotePort;
    }
    void setRemotePort(int port)
    {
        this->remotePort = port;
    }
    // ---------------------------------------------------
    int getBindPort() const
    {
        return this->bindPort;
    }
    void setBindPort(int port)
    {
        this->bindPort = port;
    }
    // ---------------------------------------------------
    int getMaxClients() const
    {
        return this->maxClients;
    }
    void setMaxClients(int maxClients)
    {
        this->maxClients = maxClients;
    }
    // ---------------------------------------------------
    int getChannels() const
    {
        return this->channels;
    }
    void setChannels(int channels)
    {
        this->channels = channels;
    }
    // ---------------------------------------------------
    int getAmountIn() const
    {
        return this->amountIn;
    }
    void setAmountIn(int amountIn)
    {
        this->amountIn = amountIn;
    }
    // ---------------------------------------------------
    int getAmountOut() const
    {
        return this->amountOut;
    }
    void setAmountOut(int amountOut)
    {
        this->amountOut = amountOut;
    }
    // ---------------------------------------------------
    int getOutConnections() const
    {
        return this->outConnections;
    }
    void setOutConnections(int outConnections)
    {
        this->outConnections = outConnections;
    }
    // ---------------------------------------------------
    int getTotalConnectedClients() const
    {
        return this->totalConnectedClients;
    }
    void setTotalConnectedClients(int totalConnectedClients)
    {
        this->totalConnectedClients = totalConnectedClients;
    }
    // ---------------------------------------------------



};
