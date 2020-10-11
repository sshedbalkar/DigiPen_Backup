#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

//
#include <WinSock2.h>

#include <sstream>
#include <vector>
//
#define BUFFER_LEN					512
#define MAX_CONCURRENT_CONNECTIONS	16

//
struct SocketAddress
{
	char IP[16];
	int Port;
};
struct ClientSocket
{
	SOCKET socket;
	sockaddr_in address;
};
//
class TCPConnection
{
public:
	TCPConnection();
	~TCPConnection();
	//
	int										Init();
	static SocketAddress					ReadSocketAddressfromFile( const char* file );
	int										ConnectToRemote( SocketAddress &remoteAddr );
	int										BindLocal( const int &port );
	int										Listen();
	int										BlockMode( unsigned long &mode );
	int										Accept( ClientSocket *&sock );
	static int								Receive( const SOCKET &sock, char *buffer, const int &bufflen );
	static bool								WouldBlock( const int &code );
	static int								CloseSocket( const SOCKET &sock );
	int										RemoveClient( ClientSocket *sock );
	static int								Send( const SOCKET &sock, const char *buffer, const int &bufflen );
	const SOCKET&							Socket();
	static bool								IsConnReset( const int &code );
	static bool								IsConnClosed( const int &code );
	static SocketAddress					CreateSocketAddrFromData( const char* ip, unsigned short port );
	static sockaddr_in						CreateSockAddr_InFromData( const char* ip, unsigned short port );
	//
private:
	int										_retCode;
	char									_bufSend[ BUFFER_LEN ];
	char									_bufRecv[ BUFFER_LEN ];
	WSAData									*_wsadata;
	sockaddr_in								*_sockAddrLocal;
	sockaddr_in								*_sockAddrRemote;
	SOCKET									_socket;
	unsigned long							_IOmode;
	typedef std::vector< ClientSocket* >	ClientSockets;
	ClientSockets							_clientSockets;
};
std::ostream& operator<<( std::ostream &out, const SocketAddress &addr );
std::ostream& operator<<( std::ostream &out, const sockaddr_in &addr );

#endif