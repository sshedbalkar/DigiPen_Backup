#include "Utility.h"
#include <algorithm>
#include <iostream>
#include <time.h>
#include "../common/ReturnCodes.h"
//
namespace utility
{
	void SplitString(const std::string& str, const char delimiter, std::vector< std::string >& result, const unsigned int &maxTokens )
	{
		unsigned int count = 0;
		std::string::size_type i = 0;
		std::string::size_type j = str.find( delimiter );
		if( j == std::string::npos )
		{
			result.push_back( str );
		}
		else
		{
			while( j != std::string::npos && count < maxTokens )
			{
				result.push_back( str.substr(i, j - i) );
				if( ++count == maxTokens )
				{
					result.push_back( str.substr(++j, str.size()) );
				}
				else
				{
					i = ++j;
					j = str.find( delimiter, j );
					if ( j == std::string::npos && count < maxTokens )
					{
						result.push_back( str.substr(i, str.size()) );
					}
				}
			}
		}
	}
	void ToUpper( std::string &str )
	{
		std::transform( str.begin(), str.end(), str.begin(), ::toupper );
	}
	void ToLower( std::string &str )
	{
		std::transform( str.begin(), str.end(), str.begin(), ::tolower );
	}
	void PrintASCII( const char *str )
	{
		unsigned int i = 0;
		while( str[ i ] != '\0' )
		{
			std::cout<<int( str[i] )<<",";
			++i;
		}
	}
	void Init( void )
	{
		::srand( (unsigned)::time(NULL) );
	}
	// generates a random float number in a range
	float RandomFloat( float min, float max )
	{
		float r = ( (float)::rand() ) / ( float )RAND_MAX;
		return min + ( r * (max - min) );
	}
	// generates a random int number in a range
	int RandomInt( int min, int max )
	{
		return ( ::rand() % (max-min) )+ min;
	}
	int ReadNextLine( char *buff, const int &buffLen, FILE *hFile )
	{
		if( ::fgets(buff, buffLen, hFile) == NULL )
		{
			if( ::ferror(hFile) )
			{
				std::cout << "[ERROR: FileData: 3]:fgets error\n";
				return RET_FAIL;
			}
		}
		return RET_SUCCESS;
	}
}