#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

//
#include <WinSock2.h>

#include <vector>
//
#define BUFFER_LEN							512
//
class UDPConnection
{
public:
											UDPConnection();
											~UDPConnection();
	//
public:
	int										Init();
	int										BindLocal( const int &port );
	static int								ReceiveFrom( const SOCKET &sock, char *buffer, const int &bufflen, sockaddr_in *remoteSockAddr );
	static int								CloseSocket( const SOCKET &sock );
	static int								SendTo( const SOCKET &sock, const char *buffer, const int &bufflen, sockaddr_in *remoteSockAddr );
	const SOCKET&							Socket();
	static bool								IsConnReset( const int &code );
	static bool								IsConnClosed( const int &code );
	static sockaddr_in*						CreateSockAddr( const char* ipAddr, unsigned short port );
	int										BlockMode( unsigned long &mode );
	//
private:
	int										_retCode;
	char									_bufSend[ BUFFER_LEN ];
	char									_bufRecv[ BUFFER_LEN ];
	WSAData									*_wsadata;
	sockaddr_in								*_sockAddrLocal;
	SOCKET									_socket;
	unsigned long							_IOmode;
};

#endif