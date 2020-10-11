#ifndef UTILITY_H
#define UTILITY_H

//
#include <vector>
#include <string>
//
#define MCR_ConcatenateHelp( a, b ) a ## b
#define MCR_Concatenate( a, b ) MCR_ConcatenateHelp( a, b )
#define MCR_Stringify( s ) #s
//
namespace utility
{
	void	SplitString(const std::string& str, const char delimiter, std::vector< std::string >& result, const unsigned int &maxTokens = -1 );
	void	ToUpper( std::string &str );
	void	ToLower( std::string &str );
	void	PrintASCII( const char *str );
	void	Init( void );
	float	RandomFloat( float min, float max );
	int		RandomInt( int min, int max );
	int		ReadNextLine( char *buff, const int &buffLen, FILE *hFile );
}

#endif