#include "UDPConnection.h"
#include <iostream>
#include "../common/ReturnCodes.h"
#include "TCPConnection.h"
//
//help the compiler figure out where winsock lives.  You could
//also add this to the linker but this means no 
//extra settings for the .csproj file
#pragma comment(lib, "ws2_32.lib")
//need for getnameinfo() function
//#include "Ws2tcpip.h"
//
UDPConnection::UDPConnection()
	:_retCode( 0 ),
	_wsadata( 0 ),
	_sockAddrLocal( 0 ),
	_socket( 0 )
{
	//
}
UDPConnection::~UDPConnection()
{
	CloseSocket( _socket );
	WSACleanup();
	//
	delete	_sockAddrLocal;
	delete	_wsadata;
}
int UDPConnection::Init()
{
	//start up winsock, asking for version 2.2
	
	//our WSAData structure.  This holds information about the version of winsock that we are using
	//not necessarily the version that we are requesting
	_wsadata = new WSAData();
	//
	SecureZeroMemory( _bufSend, BUFFER_LEN );
	SecureZeroMemory( _bufRecv, BUFFER_LEN );
	SecureZeroMemory( _wsadata, sizeof( *_wsadata ) );
	//
	//you must call this function to initialize winsock.  You must call WSACleanup when you are finished.
	//this uses a reference counter, so for each call to WSAStartup, you must call WSACleanup or suffer 
	//memory issues
	_retCode = WSAStartup( MAKEWORD( 2, 2 ), _wsadata );
	//
	//check that WSAStartup was sucessful.  
	if( _retCode != 0 )
	{
		std::cout << "Error initializing winsock!  Error was: " << _retCode << std::endl;
		return _retCode;
	}
	//
	//see what happens with this instead (uncomment)
	//WSAStartup(MAKEWORD(1,1), &wsaData);
	/*************************************
	//
	//let's examine WSADATA and see what we have here
	int majorversion = LOBYTE( _wsadata -> wVersion );
	int minorversion = HIBYTE( _wsadata -> wVersion );
	std::cout << "Version:" << majorversion << "." << minorversion << std::endl;
	//
	//the other fields in wsaData are not so important or are not used.
	//the only other one is 
	//wsaData.wHighVersion, which stores the highest version of winsock that is available
	majorversion = LOBYTE( _wsadata -> wHighVersion );
	minorversion = HIBYTE( _wsadata -> wHighVersion );
	std::cout << "Highest version available is: " << majorversion << "." << minorversion << std::endl;
	//
	***************************************/
	//now we have the version and winsock is initialized, let's start doing something useful with it

	//a sockaddr_in specifies IP address and port information.  the address family tells what the add
	//address used to bind to socket, holds port, IP, and address family
	_sockAddrLocal = new sockaddr_in();
	//
	//gethostbyaddr-  used to look up a host by its address.  otherwise identical to gethostbyname
	//gethostbyaddr(localIP, 4, AF_INET);

	//BOTH of these things are obsolete in favor of getnameinfo()
	//getnameinfo();- more on this much later
	//
	//let's create a socket!
	//this socket is not bound to anything, so all it is is an empty socket at this point
	//AF_INET- this means that we want an IP socket.  If we wanted an IPv6 socket, we would use AF_INET6
	//if we wanted to talk to an infrared port, we'd use AF_IRDA
	//bluetooth is AF_BTM

	//SOCK_STREAM - tells us the type of socket.  SOCK_STREAM is two-way, reliable, connection-based byte streams
	//that can transmit OOB data.  uses TCP.  for UDP use SOCK_DGRAM, and for raw, use SOCK_RAW.  there are others also

	//IPPROTO_TCP- tells us to use the TCP protocol.  this parameter can only be used certain ways with
	//certain address family and type settings.  if we wanted UDP, we would have had to set our type to SOCK_DGRAM 
	//or we'll get an error.  passing 0 means that we dont' care about the protocol and the socket can choose

	//NULL-  we specified TCP, so we don't need this parameter, but it would contain protocol info normally.  if
	//we enumerate all the protocols supported on this machine, we could pick one of them and pass it in here to get
	//an explicit protocol implementation.  mostly, we don't care, but if we were using IPX (for example) then we would
	//care a lot

	//0-  this is reserved, always set it to 0.  I'll ask Anthony wtf it's for

	//0-  these are the options.  normally the only one you'd care about is creating an overlapped socket for
	//use with IO ports.  if you want to do that, send WSA_IO_OVERLAPPED here.  more on that in a future demo
	
	_socket = WSASocket( AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0 );

	//error checking															-20 copy paste
	if( _socket == INVALID_SOCKET )
	{
		_retCode = WSAGetLastError();
		std::cout << "[ERR: UDPConnection: 1]: " << _retCode << std::endl;
		WSACleanup();
		return _retCode;
	}
	//
	//so now we have a socket, and we have a sockaddr_in that describes an address and port, let's put them
	//together and send some data!
	//
	return RET_SUCCESS;
}
int UDPConnection::BindLocal( const int &port )
{
	//std::cout<<"Binding to port: "<<port<<"\n";
	//
	//address family-  this identifies the addressing structure of this address structure.
	//for IP, it must always be set to AF_INET
	_sockAddrLocal->sin_family = AF_INET;
	//
	//this is the port.  remember, networks are in network-order, which is big-endian.  we use htons() to ensure
	//that the number is in the correct "endian-ness" for the network regardless of what machine architecture we're on
	//for a local connection going out, pick 0 and winsock will assign a random port
	_sockAddrLocal->sin_port = htons( port );
	//
	//now we need to set the actual IP address.  this is an in_addr struct for IPv4.  for Ipv6, it's an in6_addr
	//it is represented as a ulong with each byte representing one part of the ip address.  we can convert an address in
	//it's dot-notation (i.e. 192.168.1.1) to this format using the inet_addr() function, OR we can just use ADDR_ANY which
	//will cause this address to listen to any available network interface (machines may have more than one network card)
	//I hard-coded it to my machine's IP for testing, but let's comment that out and
	//we'll get it from our host below

	//socketAddress.sin_addr.s_addr  = inet_addr("192.168.1.107");  

	//get host.  hostent stores information about a host
	hostent* localhost;
	//
	//gethostbyname- gets host information based on host name.
	//this allocates memory only once per host no matter how many times you call
	//gethostbyname().
	//deprecated in favor of getaddrinfo

	//there are a lot of ways to do this
	localhost = gethostbyname("");
	char* localIP;

	//this will give us the four-byte representation of our ip address.
	localIP = inet_ntoa( *(in_addr*)*localhost->h_addr_list );

	//let's use the address that we got from gethostbyname for our local endpoint
	_sockAddrLocal -> sin_addr.s_addr = inet_addr( localIP );
	//
	//bind- binds a socket to an address.  
	//clientSocket- the socket to bind
	//socketAddress- the address structure to bind, need to recast to sockaddr here
	//size of the sockaddr struct
	_retCode = bind( _socket, (sockaddr*)_sockAddrLocal, sizeof(*_sockAddrLocal) );
	if( _retCode == SOCKET_ERROR )
	{
		_retCode = WSAGetLastError();
		return _retCode;
	}
	//
	std::cout<<"UDP Socket Bound: "<<*_sockAddrLocal<<"\n";
	//
	return RET_SUCCESS;
}
int UDPConnection::BlockMode( unsigned long &mode )
{
	if( _IOmode == mode )
	{
		return RET_SUCCESS;
	}
	_IOmode = mode;
	_retCode = ::ioctlsocket( _socket, FIONBIO, &mode );
	if (_retCode != NO_ERROR)
	{
		std::cout<<"Socket block mode change failed: "<<_retCode<<"\n";
	}
	return _retCode;
}
int UDPConnection::ReceiveFrom( const SOCKET &sock, char *buffer, const int &bufflen, sockaddr_in *remoteSockAddr )
{
  int size = sizeof(sockaddr_in);
	int ret;
	ret = ::recvfrom( sock, buffer, bufflen, 0, (sockaddr*)remoteSockAddr, &size);
	if( ret == SOCKET_ERROR )
	{
		ret = WSAGetLastError();
		//std::cout<<"[ERR: UDPConnection: 1]: "<<ret<<"\n";
		//ret = -1;
	}
	//
	return ret;
}
int UDPConnection::SendTo( const SOCKET &sock, const char *buffer, const int &bufflen, sockaddr_in *remoteSockAddr )
{
	int ret;
	//
	//send-  this sends
	//clientSocket- the socket to send to
	//sendbuffer - the buffer to send (char[])
	//12- number of bytes to send.  I hard-coded this because it's 2:30 am and I'm tired and
	//this is just some demo program so it's not horribly important,
	//but you probably shouldn't do that
	//0- flags.  only useful flag here is MSG_OOB, whic will send this as out of band data,
	//which for TCP will set the URG flag

	//std::cout<<"UDP: "<<(*remoteSockAddr)<<": Sending: "<<buffer<<"\n";
  int size = sizeof(sockaddr_in);
	ret = ::sendto( sock, buffer, bufflen, 0, (sockaddr*)remoteSockAddr, size);
	//if( ret != bufflen )
	//{
		//ret = ::sendto( sock, buffer + ret, bufflen - ret, 0, (sockaddr*)remoteSockAddr, sizeof(*remoteSockAddr) );
	//}
	if( ret == SOCKET_ERROR )
	{
		ret = WSAGetLastError();
		std::cout<<"Send to error: "<<ret<<"\n";
	}
	//
	return ret;
}
const SOCKET& UDPConnection::Socket()
{
	return _socket;
}
bool UDPConnection::IsConnReset( const int &code )
{
	return code == WSAECONNRESET;
}
bool UDPConnection::IsConnClosed( const int &code )
{
	return code == 0;
}
int UDPConnection::CloseSocket( const SOCKET &sock )
{
	shutdown( sock, SD_BOTH );
	closesocket( sock );
	return RET_SUCCESS;
}
sockaddr_in* UDPConnection::CreateSockAddr( const char* ipAddr, unsigned short port )
{
	sockaddr_in *sockAddr = new sockaddr_in();
	sockAddr->sin_family = AF_INET;
	sockAddr->sin_addr.s_addr = inet_addr( ipAddr );
	sockAddr->sin_port = htons( port );
	//
	return sockAddr;
}