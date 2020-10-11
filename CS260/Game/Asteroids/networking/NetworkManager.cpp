#include "NetworkManager.h"
#include "TCPConnection.h"
#include "UDPConnection.h"
#include "../common/ReturnCodes.h"
#include <iostream>
#include "../utility/Utility.h"
#include "DataPacket.h"

#pragma warning(disable:4996)

//
int ReadConfigFile( const char *config, ConfigData *data );
//
struct ConfigData
{
	std::string	ServerIP;
	unsigned short ServerPort;
  unsigned short UDPPort;
};
//
NetworkManager::NetworkManager()
	:m_UDPConn( 0 ),
	m_TCPConn( 0 ),
	m_config( 0 )
{
	//
}
NetworkManager::~NetworkManager()
{
	delete m_UDPConn;
	delete m_TCPConn;
	delete m_config;
}

/*
	Initializes the NetworkManager instance
	//
	config		file that contains configuration data
*/
int NetworkManager::Init( const char *config )
{
	m_config = new ConfigData();
	ReadConfigFile( config, m_config );
	std::cout<<"IP: "<<m_config->ServerIP<<", Port: "<<m_config->ServerPort<<", port: "<< std::endl;
	//
	
  m_remoteAddress.sin_family = AF_INET;
  m_remoteAddress.sin_port = htons(m_config->ServerPort);
  m_remoteAddress.sin_addr.s_addr = inet_addr(m_config->ServerIP.c_str());
  
  SecureZeroMemory(&m_serverAddr, sizeof(sockaddr_in));
  m_serverAddr.sin_family = AF_INET;
  m_serverAddr.sin_port = htons(m_config->UDPPort);
  m_serverAddr.sin_addr.s_addr = inet_addr(m_config->ServerIP.c_str());

	int ret;
	m_TCPConn = new TCPConnection();
	if( (ret = m_TCPConn->Init()) != RET_SUCCESS )
	{
		return RET_FAIL;
	}
	m_TCPConn->BindLocal( 0 );
	//
	m_UDPConn = new UDPConnection();
	if( (ret = m_UDPConn->Init()) != RET_SUCCESS )
	{
		std::cout<<"UDP init was not successful\n";
		return RET_FAIL;
	}
  m_UDPConn->BindLocal( m_config->UDPPort );
	unsigned long m2 = 1L;
	m_UDPConn->BlockMode( m2 );
	//
	SocketAddress addr = TCPConnection::CreateSocketAddrFromData( m_config->ServerIP.c_str(), m_config->ServerPort );
	if( m_TCPConn->ConnectToRemote( addr ) == RET_SUCCESS )
	{
		std::cout<<"Successfully connected to the server\n";
		m2 = 1L;
		m_TCPConn->BlockMode( m2 );
	}
	else
	{
		std::cout<<"Error connecting to server\n";
		return RET_FAIL;
	}
	//
	std::cout<<"Init success\n";
	return RET_SUCCESS;
}

sockaddr_in&  NetworkManager::GetUPDServerAddr(void)
{
  return m_serverAddr;
}

/*
	Receives a msg from a TCP server in a blocking way.
	//
	str			string object where the received msg will be copied into
*/
int NetworkManager::RecvTCPMsgBlocking( std::string &str )
{
	int ret;
	do
	{
		ret = RecvTCPMsg( str );
	}
	while( str.empty() );
	//
	return ret;
}
/*
	Receives a msg from a TCP server in a NON-blocking way.
	//
	str			string object where the received msg will be copied into
*/
int NetworkManager::RecvTCPMsg( std::string &str )
{
	int ret = TCPConnection::Receive( m_TCPConn->Socket(), m_buffer, BUFF_LEN );
	if( TCPConnection::WouldBlock(ret) )
	{
		//
	}
	else if( TCPConnection::IsConnClosed(ret) || TCPConnection::IsConnReset(ret) )
	{
		std::cout<<"Server has disconnected0\n";
		return RET_SERVER_OFFLINE;
	}
	else
	{
		str.assign( m_buffer, ret - 1 );
		//std::cout<<"[Server]: "<<str.c_str()<<", Len: "<<ret<<"\n";
		return RET_SUCCESS;
	}
	//
	return ret;
}

/*
	Receives a msg from a UDP system in a blocking way.
	//
	str			string object where the received msg will be copied into
*/
int NetworkManager::RecvUDPMsgBlocking( std::string &str )
{
	int ret;
	do
	{
		ret = RecvUDPMsg( str );
	}
	while( str.empty() );
	//
	return ret;
}

/*
	Receives a msg from a UDP system in a NON-blocking way.
	//
	str			string object where the received msg will be copied into
*/
int NetworkManager::RecvUDPMsg( std::string &str )
{
  sockaddr_in pleaseWork;
	::SecureZeroMemory( &pleaseWork, sizeof(m_remoteAddress) );
	int ret = UDPConnection::ReceiveFrom( m_UDPConn->Socket(), m_UDPRecvBuff, UDP_BUFF_LEN, &pleaseWork);
	//std::cout<<"Ret: "<<ret<<"\n";
	if( TCPConnection::WouldBlock(ret) )
	{
		//
	}
	else if( TCPConnection::IsConnClosed(ret) || TCPConnection::IsConnReset(ret) )
	{
		std::cout<<"User has disconnected0\n";
	}

	else
	{
		str.assign( m_UDPRecvBuff, ret );
		return RET_SUCCESS;
	}
	return RET_FAIL;
}

/*
	returns the sockaddr_in object of the remote system from where the last UDP msg was received
*/
sockaddr_in& NetworkManager::GetLastRemoteAddr( void )
{
	return m_remoteAddress;
}

/*
	sends a UDP msg
	//
	str		c-string to be sent
	addr	address of the remote system
*/
int NetworkManager::SendUDPMsg( const char *str, sockaddr_in &addr )
{
	return SendUDPMsg( std::string(str), addr );
}

int NetworkManager::SendUDPMsg( const char *str, int size, sockaddr_in &addr )
{
  std::string m(str, size);
  return SendUDPMsg(m, addr);
}

/*
	sends a UDP msg
	//
	str		string to be sent
	addr	address of the remote system
*/
int NetworkManager::SendUDPMsg( const std::string &str, sockaddr_in &addr )
{
	//int start = 0, len = 0, ret;
	//
	/*
	// code to send typed data as 255 byte chunks
	do
	{
		len = str.copy( _UDPSendBuff, BUFF_LEN-1, start );
		_UDPSendBuff[len] = '\0';
		start += len;
		//
		//std::cout<<"Sending: "<<_buffer<<"\n";
		ret = UDPConnection::SendTo( _UDPConn->Socket(), _UDPSendBuff, ++len, &addr );
	}
	while( len == BUFF_LEN );
	*/
	return UDPConnection::SendTo( m_UDPConn->Socket(), str.c_str(), str.size(), &addr );
	//return RET_SUCCESS;
}

/*
	sends a TCP msg
	//
	str		c-string to be sent
*/
int NetworkManager::SendTCPMsg( const char *str )
{
	return SendTCPMsg( std::string(str) );
}

/*
	sends a TCP msg
	//
	str		string to be sent
*/
int NetworkManager::SendTCPMsg( const std::string &str )
{
	int start = 0, len = 0, ret;
	//
	// code to send typed data as 255 byte chunks
	do
	{
		len = str.copy( m_buffer, BUFF_LEN-1, start );
		m_buffer[len] = '\0';
		start += len;
		//
		//std::cout<<"Sending: "<<_buffer<<"\n";
		ret = TCPConnection::Send( m_TCPConn->Socket(), m_buffer, ++len );
	}
	while( len == BUFF_LEN );
	return RET_SUCCESS;
}

SOCKET NetworkManager::GetUPDSocket()
{
  return  m_UDPConn->Socket();
}

int ReadConfigFile( const char *config, ConfigData *data )
{
	FILE *file;
	const int BuffLen = 100;
	char line[ BuffLen ];
	//
	if ( ::fopen_s(&file, config, "r") != 0 )
	{
		std::cout << "[ERROR: Client: 1]: "<<config<<" could not be opened for reading\n" << "\n";
		return RET_FAIL;
	}
	//
	if( utility::ReadNextLine( line, BuffLen, file) != RET_SUCCESS )
	{
		::fclose( file );
		return RET_FAIL;
	}
	data->ServerIP.assign( line );
	data->ServerIP.erase( --data->ServerIP.end() );
	//
	if( utility::ReadNextLine( line, BuffLen, file) != RET_SUCCESS )
	{
		::fclose( file );
		return RET_FAIL;
	}
	data->ServerPort = ::atoi( line );
	//
  if( utility::ReadNextLine( line, BuffLen, file) != RET_SUCCESS )
	{
		::fclose( file );
		return RET_FAIL;
	}
  data->UDPPort = ::atoi( line );
	::fclose( file );
	//std::cout<<"Server: ["<<data->ServerIP.c_str()<<": "<<data->ServerPort<<"], Client: "<<data->ClientPort<<", Recv: "<<data->RecvDir.c_str()<<", Send: "<<data->SendDir.c_str()<<"\n";
	return RET_SUCCESS;
}