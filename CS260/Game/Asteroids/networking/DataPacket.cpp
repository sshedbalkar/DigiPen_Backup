#include "DataPacket.h"
#include <iostream>
#include "../common/ReturnCodes.h"
#include "../utility/Utility.h"
//
/*
	constructs a default Datapacket object
*/
DataPacket::DataPacket()
	:m_stream( std::ostringstream::binary )
{
	//
}

/*
	Constructs a Datapacket object from a delimited data packet received over network - c-string style
	ex: 123%34.45%sss%Hello
*/
DataPacket::DataPacket( const char* data )
	:m_stream( std::string(data), std::ostringstream::binary )
{
	//
}

/*
	Constructs a Datapacket object from a delimited data packet received over network - string style
	ex: 123%34.45%sss%Hello
*/
DataPacket::DataPacket( std::string& data )
	:m_stream( data, std::ostringstream::binary )
{
	//
}
DataPacket::~DataPacket()
{
	//
}

/*
	returns the string copy of the associated data. Effectively removes the delimiter at the last position from the copied data
*/
std::string DataPacket::Getdata()
{
	std::string &str = m_stream.str();
	if( !str.empty() )
	{
		str.erase( --str.end() );
	}
	return str;
}

/*
	returns the string copy of the associated data. DOES not remove the delimiter at the last position from the copied data
*/
std::string DataPacket::GetdataUntrimmed()
{
	return m_stream.str();
}

/*
	returns a std::vector of strings from splitting the contents of the stream
	ex: Stream: 123%34.45%sss%Hello
	return value: [123, 34.45, sss, Hello]
*/
std::vector< std::string > DataPacket::Split()
{
	std::vector< std::string > vec;
	//
	utility::SplitString( GetdataUntrimmed(), DELIMITER[0], vec );
	//
	return vec;
}