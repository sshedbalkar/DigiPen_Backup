#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <fstream>
#include <string>
#include "winsock2.h"
#include <list>
#include <ctime>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

//TCP buffer length
const unsigned BUFFER_LENGTH = 256;

//Max players
const int MAX_PLAYERS = 4;

//Commands
const char * quitCommand = "/quit";

struct Client
{
  unsigned char ID;
  SOCKET sock;
  sockaddr_in address;
};

struct PlayerData
{
  bool isConnected;
  bool playerHasWon;
  bool firedBullet;
  float positionX;
  float positionY;
  float rotation;
};

void RespondToSocketError(int result,
                          bool terminateOverride,
                          SOCKET listenerSocket = 0,
                          SOCKET UDPSocket = 0,
                          std::list<Client *> * clients = 0,
                          HANDLE quitHandle = 0); //Prototype

DWORD WINAPI TerminateServer(LPVOID param); //Prototype

int main(void)
{
  //Declare variables
  int result = 0;
  bool inLobby = true;
  WSADATA wsData;
  PlayerData previousPacket[MAX_PLAYERS];
  SecureZeroMemory(previousPacket, sizeof(previousPacket));

  std::string asteroidsCommand("/ASTEROIDS");
  asteroidsCommand.push_back('\0');

  srand( (unsigned)time(NULL) );

  //Host information
  char * localIP;
  hostent * localHost;

  //Initialize buffer
  char buffer[BUFFER_LENGTH];
  SecureZeroMemory(buffer, BUFFER_LENGTH);

  //Initialize WinSock
  result = WSAStartup(MAKEWORD(2,2), &wsData);
	if(result)
  {
		std::cout << "Error starting WinSock. Error code: " << result;
		return result;
	}

  //Read data from "config.txt."
  std::string inStr;
  std::ifstream infile("config.txt");
  if(!infile.is_open())
  {
    std::cout << "Could not open \"config.txt\"";
    WSACleanup();
    return -1;
  }

  //Skip over the IP address and read the desired TCP port, then UDP port.
  infile >> inStr;
  infile >> inStr;
  short TCP_port = atoi(inStr.c_str());
  infile >> inStr;
  short UDP_port = atoi(inStr.c_str());

  //Initialize the data for the listener socket
  SOCKET listenerSocket;
  struct sockaddr_in TCP_local_addr;
  localHost = gethostbyname("");
  localIP = inet_ntoa(*(in_addr*)*localHost->h_addr_list);

  TCP_local_addr.sin_family = AF_INET;
  TCP_local_addr.sin_port = htons(TCP_port);
  TCP_local_addr.sin_addr.s_addr  = inet_addr(localIP);

  struct sockaddr_in UDP_local_addr;
  localHost = gethostbyname("");
  localIP = inet_ntoa(*(in_addr*)*localHost->h_addr_list);

  UDP_local_addr.sin_family = AF_INET;
  UDP_local_addr.sin_port = htons(UDP_port);
  UDP_local_addr.sin_addr.s_addr  = inet_addr(localIP);

  //Debugging information
  std::cout << "Host Name: " << localHost->h_name << std::endl;
  std::cout << "Your IP Address: " << localIP << std::endl;
  std::cout << "TCP Port: " << TCP_port << std::endl;
  std::cout << "UDP Port: " << UDP_port << std::endl;
  std::cout << "Input \"" << quitCommand << "\" to gracefully terminate the server." << std::endl;

  //Create the listener socket
  listenerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
  if(listenerSocket == INVALID_SOCKET)
    RespondToSocketError(SOCKET_ERROR, true, listenerSocket, 0, 0, 0);

  //Set the listener socket to non-blocking mode
  u_long iMode = 1;
  result = ioctlsocket(listenerSocket, FIONBIO, &iMode);
  RespondToSocketError(result, false, listenerSocket, 0, 0, 0);

  //Bind the listener socket with our local information
  result = bind(listenerSocket, (SOCKADDR*)&TCP_local_addr, sizeof(TCP_local_addr));
  RespondToSocketError(result, false, listenerSocket, 0, 0, 0);

  //Create the UDP socket
  SOCKET UDPSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
  if(UDPSocket == INVALID_SOCKET)
    RespondToSocketError(SOCKET_ERROR, true, listenerSocket, UDPSocket, 0, 0);

  //Set the UDP socket to non-blocking mode
  result = ioctlsocket(UDPSocket, FIONBIO, &iMode);
  RespondToSocketError(result, false, listenerSocket, UDPSocket, 0, 0);

  //Bind the UDP socket to our local information
  result = bind(UDPSocket, (SOCKADDR*)&UDP_local_addr, sizeof(UDP_local_addr));
  RespondToSocketError(result, false, listenerSocket, UDPSocket, 0, 0);

  //Prepare list of clients and IDs
  std::list<Client *> clients;
  bool IDs[MAX_PLAYERS];
  SecureZeroMemory(IDs, MAX_PLAYERS);

  //Go to listening state. Backlog of 1.
  result = listen(listenerSocket, 1);
  RespondToSocketError(result, false, listenerSocket, UDPSocket, &clients, 0);

  std::cout << "Now in listening state. Ready to receive clients...\n";

  //Begin quit thread
  HANDLE QuitHandle = CreateThread(NULL, 0, TerminateServer, NULL, 0, NULL);

  //Main operational loop
  do
  {
    //Check on the status of QuitHandle
    if(WaitForSingleObject(QuitHandle, 0) == WAIT_OBJECT_0)
      RespondToSocketError(SOCKET_ERROR, true, listenerSocket, UDPSocket, &clients, QuitHandle);

    //If we're in the lobby
    if(inLobby)
    {
      //Prepare a remote socket for an attempted connection.
      SOCKET remoteSocket;
      sockaddr_in remoteEndpoint;
      int endpointSize = sizeof(remoteEndpoint);
      SecureZeroMemory(&remoteEndpoint, endpointSize);
    
      //Attempt a remote connection.
      remoteSocket = accept(listenerSocket, (sockaddr*)&remoteEndpoint, &endpointSize);

      //If SOCKET_ERROR was returned, error check...
      if(remoteSocket == SOCKET_ERROR)
        RespondToSocketError(SOCKET_ERROR, false, listenerSocket, UDPSocket, &clients, QuitHandle);

      //If a connection was established however...
      else
      {
        //Create the new client
        Client * newClient = new Client;
        newClient->sock = remoteSocket;
        newClient->address.sin_addr = remoteEndpoint.sin_addr;
        newClient->address.sin_family = AF_INET;
        newClient->address.sin_port = htons(UDP_port);

        //Find the first available ID
        bool foundID = false;
        for(int i = 0; i < MAX_PLAYERS; i++)
        {
          //If the current ID is not taken...
          if(!IDs[i])
          {
            //Take it
            IDs[i] = true;

            //Set the ID to it
            newClient->ID = i;

            //Mark the local flag
            foundID = true;

            //Don't keep looking
            break;
          }//end "current ID is not taken"
        }//end search through IDs

        //If no ID was available...
        if(!foundID)
        {
          //...End the connection.
          shutdown(remoteSocket, SD_BOTH);
          closesocket(remoteSocket);
          delete newClient;
        }
        else
        {
          //Store it in the list
          clients.push_front(newClient);
          std::cout << "Connected to Client #" << (int)newClient->ID << std::endl;
        }
      }//close "connection with new client established"

      //Iterate through the clients and try to receive TCP data from each one
      for(std::list<Client*>::iterator recv_it = clients.begin(); recv_it != clients.end(); recv_it++)
      {
        int bytesReceived = recv((*recv_it)->sock, buffer, BUFFER_LENGTH, 0);
        if(bytesReceived == SOCKET_ERROR)
        {
          if(WSAGetLastError() == WSAECONNRESET)
            bytesReceived = 0;
          else
            RespondToSocketError(bytesReceived, false, listenerSocket, UDPSocket, &clients, QuitHandle);
        }

        //If they've disconnected...
        if(bytesReceived == 0)
        {
          //Display that they've left
          std::cout << "Client #" << (int)(*recv_it)->ID << " has disconnected\n";

          //Free up their ID for reuse
          IDs[(*recv_it)->ID] = false;

          //Close the socket
          shutdown((*recv_it)->sock, SD_BOTH);
          closesocket((*recv_it)->sock);

          //Delete the Client *
          delete *recv_it;

          //Erase the entry
          clients.erase(recv_it);

          //Restart the loop (because the iterator is now invalid)
          break;
        }//end "client has disconnected"

        //If data was received...
        if(bytesReceived > 0)
        {
          //Put it in a string and display it
          std::string str(buffer, bytesReceived);
          std::cout << "Client #" << (int)(*recv_it)->ID << " says: " << str << std::endl;

          //If it wasn't the command to start a game, send it back to each client
          if(str != asteroidsCommand)
          {
            //Put the ID of the client who sent it in the first byte of the buffer,
            //and then put every other byte after it in the buffer. The client promises
            //not to send messages that would overflow the buffer, so this is safe.
            buffer[0] = (*recv_it)->ID;
            memcpy(buffer + 1, str.c_str(), str.size());

            //Send the new buffer to every other client
            for(std::list<Client*>::iterator send_it = clients.begin(); send_it != clients.end(); send_it++)
            {
              result = send((*send_it)->sock, buffer, bytesReceived + 1, 0);
              RespondToSocketError(result, false, listenerSocket, UDPSocket, &clients, QuitHandle);
            }
          }

          //If it was the command to start a game
          if(str == asteroidsCommand)
          {
            //If more than 1 person is connected
            if(clients.size() > 1)
            {
              //Leave the lobby
              inLobby = false;

              //Indicate that it has recognized the command
              #ifdef _DEBUG
              std::cout << "Acknowledging " << str << " command. Leaving lobby." << std::endl;
              #endif

              //Generate a uniform seed
              unsigned char seed = (unsigned char)rand();

              //Send a start game command to each client
              for(std::list<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
              {
                //Generate the message from the original string
                std::string message(str);

                //Remove the null-terminator
                message.pop_back();

                //Add a colon deliminator
                message.push_back(':');

                //Add the seed
                message.push_back(seed);

                //Add a second colon deliminator
                message.push_back(':');

                //Add this client's ID
                message.push_back((*it)->ID);

                //Add a third colon deliminator
                message.push_back(':');

				//Add the number of connected clients
				unsigned char numClients = (unsigned char)clients.size();
				message.push_back(numClients);

				//Add a fourth colon deliminator
				message.push_back(':');

                //Send the message over TCP
                result = send((*it)->sock, message.c_str(), message.size(), 0);
                RespondToSocketError(result, false, listenerSocket, UDPSocket, &clients, QuitHandle);
              }

              //Exit the loop
              break;
            }//end "if more than one client is connected"

            //If only one client is connected
            else
            {
              //Send an error message back
              std::list<Client*>::iterator onlyClient = clients.begin();
              strcpy(buffer, " SERVER: YOU ARE THE ONLY CONNECTED PLAYER ");
              result = send((*onlyClient)->sock, buffer, strlen(buffer), 0);
              RespondToSocketError(result, false, listenerSocket, UDPSocket, &clients, QuitHandle);
              #ifdef _DEBUG
              std::cout << "I say:" << buffer << std::endl;
              #endif
            }//end "only one client is connected"
          }//end "command to start a game"
        }//end "data was received"
      }//end "receive TCP data from all clients"
    }//end "in lobby"

    //If we're in a game
    else
    {
      //Generate a new packet to be sent this iteration
      PlayerData packet[MAX_PLAYERS];
      //Store the number of "special" entries that are being sent
      int dummyEntries = 0;

      //Check on the TCP connection with each connected client, and if they've
      //disconnected, prune them from the list of clients.
      for(std::list<Client*>::iterator it = clients.begin(); it != clients.end(); /*no action*/ )
      {
        result = recv((*it)->sock, buffer, BUFFER_LENGTH, 0);
        if(result == SOCKET_ERROR)
        {
          if(WSAGetLastError() == WSAECONNRESET)
            result = 0;
          else
            RespondToSocketError(result, false, listenerSocket, UDPSocket, &clients, QuitHandle);
        }

        //If they've disconnected...
        if(result == 0)
        {
          //Display that they've left
          std::cout << "Client #" << (int)(*it)->ID << " has disconnected\n";

          //Free up their ID for reuse
          IDs[(*it)->ID] = false;

          //Close the socket
          shutdown((*it)->sock, SD_BOTH);
          closesocket((*it)->sock);

          //Delete the Client *
          delete *it;

          //Erase the entry
          clients.erase(it);

          //Restart the loop
          it = clients.begin();
        }//end "client has disconnected"
        else
          it++;
      }//close "prune disconnected clients"

      //If all previously connected clients were pruned, return to the lobby
      if(clients.size() == 0)
      {
        std::cout << "All clients disconnected. Returning to lobby." << std::endl;
        inLobby = true;
        continue;
      }

      /*
        At this point in program flow, 'clients' is guaranteed to have at least
        one connected client, and contain no invalid clients. Now we need to
        fill in the packet we made earlier with the special "not really
        connected" data for every entry that represents a disconnected client.
      */
      for(int i = 0; i < MAX_PLAYERS; i++)
      {
        //If this index represents an ID which isn't taken...
        if(!IDs[i])
        {
          dummyEntries++;
          packet[i].firedBullet = false;
          packet[i].isConnected = false;
          packet[i].playerHasWon = false;
          packet[i].positionX = -1;
          packet[i].positionY = -1;
          packet[i].rotation = -1;
        }//end "ID is not taken"
      }//end "fill with special data"

      /*
        Now only the elements in 'packet' which represent connected players
        aren't filled in. Iterate through 'clients' and receive data from
        each of them
      */
      for(std::list<Client*>::iterator recv_it = clients.begin(); recv_it != clients.end(); recv_it++)
      {
        int addrLen = sizeof(sockaddr_in);
        result = recvfrom(UDPSocket, (char*)(packet + (*recv_it)->ID), sizeof(PlayerData), 0, (sockaddr*)&(*recv_it)->address, &addrLen);

        //If no data was received for some reason, or too much data was received...
        if(WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEMSGSIZE)
        {
          //Fill in their entry with "disconnected" data in this packet
          dummyEntries++;
          packet[(*recv_it)->ID].firedBullet = false;
          packet[(*recv_it)->ID].isConnected = false;
          packet[(*recv_it)->ID].playerHasWon = false;
          packet[(*recv_it)->ID].positionX = -1;
          packet[(*recv_it)->ID].positionY = -1;
          packet[(*recv_it)->ID].rotation = -1;

          #ifdef _DEBUG
          std::cout << "Received nothing from Client #" << (int)(*recv_it)->ID << std::endl;
          #endif
        }//end "no data was received"

        #ifdef _DEBUG
        if(result > 0)
        {
          //If it didn't error and we received something, print how much was received
          std::cout << "Received " << result << " bytes from Client #" << (int)(*recv_it)->ID;
        }
        #endif

      }//end "iterate over clients and recv from them"

      /*
        The packet now contains all relevant information about the clients who
        are still connected. If it is different from the previous packet, send
        the packet to each client to conclude this iteration.
      */
      if(memcmp(packet, previousPacket, sizeof(packet)) != 0)
      {
        for(std::list<Client*>::iterator send_it = clients.begin(); send_it != clients.end(); send_it++)
        {
          #ifdef _DEBUG
          std::cout << "Sending " << sizeof(PlayerData) * MAX_PLAYERS << " bytes with " << dummyEntries << " dummy entries to Client #" << (int)(*send_it)->ID << std::endl;
          #endif

          int size = sizeof(sockaddr_in);

          result = sendto( UDPSocket,
                           (char*)packet,
                           sizeof(PlayerData) * MAX_PLAYERS,
                           0,
                           (sockaddr*)&(*send_it)->address,
                           size);

          RespondToSocketError(result, false, listenerSocket, UDPSocket, &clients, QuitHandle);

        }//close "send packet to all clients"
      }//close "if this packet is different from the previous packet"

      //Store the current packet in 'previousPacket'
      memcpy(previousPacket, packet, sizeof(packet));
    }//close "in a game"

    //Sleep for the rest of the time slice
    Sleep(1);

  }while(true); //close "main operational loop"

  RespondToSocketError(SOCKET_ERROR, true, listenerSocket, UDPSocket, &clients, QuitHandle);
  return 0;
}

DWORD WINAPI TerminateServer(LPVOID param)
{
  std::string input;
  do
  {
    std::cin >> input;
  } while(input != quitCommand);

  return 0;
}

/*
  Responding to socket error is a lot of copy and paste code, so I'm encapsulating it in this function.
  
  -result is the result of the last call to a function that may have returned SOCKET_ERROR.
  -terminateOverride specifies whether this should forcibly terminate the program or not, despite any socket error conditions
  -listenerSocket is the listener socket that will need to be closed.
  -UDPSocket is the UPD socket that will need to be closed.
  -clients is the list of clients that will need to be freed and have their Sockets closed.
  -quitHandle is the handle to the quit thread that will need to be closed.
*/
void RespondToSocketError(int result, bool terminateOverride, SOCKET listenerSocket, SOCKET UDPSocket, std::list<Client *> * clients, HANDLE quitHandle)
{
  if((result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) || terminateOverride)
  {
    std::cout << "Socket Error. Error Code: " << WSAGetLastError() << std::endl;

    if(listenerSocket)
    {
      shutdown(listenerSocket, SD_BOTH);
      closesocket(listenerSocket);
    }

    if(UDPSocket)
    {
      shutdown(UDPSocket, SD_BOTH);
      closesocket(UDPSocket);
    }

    if(clients)
    {
      for(std::list<Client *>::iterator it = clients->begin(); it != clients->end(); it++)
      {
        shutdown((*it)->sock, SD_BOTH);
        closesocket((*it)->sock);
        delete *it;
      }
    }

    if(quitHandle)
      CloseHandle(quitHandle);

    WSACleanup();

    #ifdef _DEBUG
    system("pause");
    #endif

    exit(WSAGetLastError());
  }
}