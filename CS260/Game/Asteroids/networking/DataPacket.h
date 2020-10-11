#pragma once
//
#include <sstream>
#include "CommandCodes.h"
#include <string>
#include <vector>
//
class DataPacket
{
public:
								DataPacket();
								DataPacket( const char* data );
								DataPacket( std::string& data );
	virtual						~DataPacket();
	//
	std::string					Getdata();
	std::string					GetdataUntrimmed();
	std::vector< std::string >	Split();
	//
	/*
		Appends a value to the stream. Also appends a delimiter after each append
		ex: input: "abc"
		output: "abc%"
	*/
	template< typename T >
	DataPacket&					operator<<( const T &t )
	{
		m_stream<<t<<DELIMITER;
		return *this;
	}
	//
private:
	std::ostringstream			m_stream;
};