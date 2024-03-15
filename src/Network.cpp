#include "Network.h"

////! send string
//int Network::sendPacketToClient(string strPacket)
//{
//    ENetPacket * packet = enet_packet_create (strPacket.c_str(),
//                          strPacket.length() + 1,
//                          ENET_PACKET_FLAG_RELIABLE);
//    enet_peer_send (peerClient, 0, packet);
//    return 0;
//}
//
////! send vector of ints
//int Network::sendPacketToClient(vector<int> serialized)
//{
//    /* Create a reliable packet of size 7 containing "packet\0" */
//    ENetPacket * packet = enet_packet_create (&serialized[0],
//                          serialized.size() * sizeof(int),
//                          ENET_PACKET_FLAG_RELIABLE);
//    enet_peer_send (peerClient, 0, packet);
//    return 0;
//}
////! send int
//int Network::sendPacketToServer(int value)
//{
//    ENetPacket * packet = enet_packet_create (&value,
//                          sizeof(int) + 1,
//                          ENET_PACKET_FLAG_RELIABLE);
//    enet_peer_send (peerServer, 0, packet);
//    return 0;
//}
//////! send string
////int Network::sendPacketToServer(string strPacket)
////{
////    ENetPacket * packet = enet_packet_create (strPacket.c_str(),
////                          strPacket.length() + 1,
////                          ENET_PACKET_FLAG_RELIABLE);
////    enet_peer_send (peerServer, 0, packet);
////    return 0;
////}
////
////! send vector of ints
//int Network::sendPacketToServer(vector<int> serialized)
//{
//    /* Create a reliable packet of size 7 containing "packet\0" */
//    ENetPacket * packet = enet_packet_create (&serialized[0],
//                          (serialized.size() * sizeof(int)) + 1,
//                          ENET_PACKET_FLAG_RELIABLE);
//    enet_peer_send (peerServer, 0, packet);
//    return 0;
//}
//
//////! receive vector of ints from server
////vector<int> Network::receivePacketFromServer()
////{
////    vector<int> receivedData{0};
////    /* Wait up to 1000 milliseconds for an event. */
////    while (enet_host_service (client, &event, 0) > 0)
////    {
////        switch (event.type)
////        {
////        case ENET_EVENT_TYPE_RECEIVE:
////            sprintf ((char*)stream, "received len %u s %s from %s ch. %u.\n",
////                     event.packet -> dataLength,
////                     event.packet -> data,
////                     event.peer -> data,
////                     event.channelID);
////            cout << stream << endl;
////
////            receivedData.reserve(event.packet -> dataLength);
////            receivedData = static_cast<vector<int>>(*event.packet->data);
////
////            /* Clean up the packet now that we're done using it. */
////            enet_packet_destroy (event.packet);
////            break;
////        case ENET_EVENT_TYPE_DISCONNECT:
////            sprintf ((char*)stream, "%s disconnected.\n", (void*)event.peer -> data);
////            cout << stream << endl;
////            /* Reset the peer's client information. */
////            event.peer -> data = NULL;
////            break;
////        }
////    }
////
////    return receivedData;
////}
//
//
////! receive int value
////! return 0 no data, -1 disconnected, >0 value received
//int Network::receivePacketFromClient()
//{
//    int receivedValue = 0;
//    /* Wait up to 1000 milliseconds for an event. */
//    while (enet_host_service (server, &event, 0) > 0)
//    {
//        switch (event.type)
//        {
//        case ENET_EVENT_TYPE_RECEIVE:
//            sprintf ((char*)stream, "len %u s %d from %s ch. %u.\n",
//                     event.packet -> dataLength,
//                     event.packet -> data,
//                     event.peer -> data,
//                     event.channelID);
//            cout << stream << endl;
//
//            receivedValue = static_cast<int>(*event.packet->data);
//
//            /* Clean up the packet now that we're done using it. */
//            enet_packet_destroy (event.packet);
//            return receivedValue;
//
//        case ENET_EVENT_TYPE_DISCONNECT:
//            sprintf ((char*)stream, "%s disconnected.\n", (void*)event.peer -> data);
//            cout << stream << endl;
//            /* Reset the peer's client information. */
//            event.peer -> data = NULL;
//            return -1;
//        }
//    }
//    return 0;
//}
//
//
////! receive int value
////! return 0 no data, -1 disconnected, >0 value received
//int Network::receivePacketFromServer()
//{
//    int receivedValue = 0;
//    /* Wait up to 1000 milliseconds for an event. */
//    while (enet_host_service (client, &event, 0) > 0)
//    {
//        switch (event.type)
//        {
//        case ENET_EVENT_TYPE_RECEIVE:
//            sprintf ((char*)stream, "len %u Cmd %u from %s ch. %u.\n",
//                     event.packet -> dataLength,
//                     event.packet -> data,
//                     event.peer -> data,
//                     event.channelID);
//            cout << stream << endl;
//
//            receivedValue = static_cast<int>(*event.packet->data);
//
//            /* Clean up the packet now that we're done using it. */
//            enet_packet_destroy (event.packet);
//            return receivedValue;
//
//        case ENET_EVENT_TYPE_DISCONNECT:
//            sprintf ((char*)stream, "%s disconnected.\n", (void*)event.peer -> data);
//            cout << stream << endl;
//            /* Reset the peer's client information. */
//            event.peer -> data = NULL;
//            return -1;
//        }
//    }
//    return 0;
//}
//
//
//////! receive vector of ints from server
////vector<int> Network::receivePacketFromClient()
////{
////    vector<int> receivedData{0};
////    /* Wait up to 1000 milliseconds for an event. */
////    while (enet_host_service (server, &event, 0) > 0)
////    {
////        switch (event.type)
////        {
////        case ENET_EVENT_TYPE_RECEIVE:
////            sprintf ((char*)stream, "len %u s %s from %s ch. %u.\n",
////                     event.packet -> dataLength,
////                     event.packet -> data,
////                     event.peer -> data,
////                     event.channelID);
////            cout << stream << endl;
////
////            receivedData.reserve(event.packet -> dataLength);
////            receivedData = static_cast<vector<int>>(*event.packet->data);
////
////            /* Clean up the packet now that we're done using it. */
////            enet_packet_destroy (event.packet);
////            break;
////        case ENET_EVENT_TYPE_DISCONNECT:
////            sprintf ((char*)stream, "%s disconnected.\n", (void*)event.peer -> data);
////            cout << stream << endl;
////            /* Reset the peer's client information. */
////            event.peer -> data = NULL;
////            break;
////        }
////    }
////
////    return receivedData;
////}
//
//
