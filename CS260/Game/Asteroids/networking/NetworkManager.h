#pragma once
//
#include <WinSock2.h>
#include <string>
//
#define		BUFF_LEN		256
#define		UDP_BUFF_LEN	2048
//
struct		ConfigData;
class		TCPConnection;
class		UDPConnection;
//
class NetworkManager
{
public:
							NetworkManager();
	virtual					~NetworkManager();
	//
	int						Init( const char *config );
	int						RecvTCPMsgBlocking( std::string &str );
	int						RecvTCPMsg( std::string &str );
	int						SendUDPMsg( const char *str, sockaddr_in &addr );
  int						SendUDPMsg( const char *str, int size, sockaddr_in &addr );
	int						SendUDPMsg( const std::string &str, sockaddr_in &addr );
	int						SendTCPMsg( const char *str );
	int						SendTCPMsg( const std::string &str );
	int						RecvUDPMsg( std::string &str );
	int						RecvUDPMsgBlocking( std::string &str );
	sockaddr_in&	GetLastRemoteAddr( void );
  sockaddr_in&  GetUPDServerAddr(void);
  SOCKET        GetUPDSocket();

	//
private:
	ConfigData*				m_config;
	TCPConnection*			m_TCPConn;
	UDPConnection*			m_UDPConn;
	char					m_buffer[ BUFF_LEN ];
	char					m_UDPSendBuff[ BUFF_LEN ];
	char					m_UDPRecvBuff[ UDP_BUFF_LEN ];
	sockaddr_in				m_remoteAddress;
  sockaddr_in   m_serverAddr;
};