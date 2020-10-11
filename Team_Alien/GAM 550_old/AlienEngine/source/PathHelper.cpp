#include <string>
#include <fstream>
#include "PathHelper.h"

namespace Framework
{

//================================================================
void PathHelper::PathGetExtension( const std::string& path, std::string& ext )
{
	unsigned int i = path.size() - 1;
	ext = "";

	i = path.find_last_of( '.' );
	if ( i <= 0 ) return;
	++i;
	while ( i < path.size() )
	{
		ext.push_back( path[i] );
		++i;
	}
}
//================================================================
void PathHelper::PathGetFilename( const std::string& path, std::string& filename )
{
	unsigned int i = path.find_last_of( '\\' );
	filename = "";
	++i;
	while ( path[i] != '.' && i < path.size() )
	{
		filename.push_back( path[i] );
		++i;
	}
}
//================================================================
void PathHelper::PathChangeExtension( std::string& path, const std::string& ext )
{
	unsigned int i = path.find_last_of( '.' );

	path = path.substr( 0, i + 1 );
	path += ext;

}
//================================================================
void PathHelper::StringToUpper( std::string& str )
{
	for ( unsigned int i = 0; i < str.size(); ++i )
	{
		if ( str[i] >= 'a' && str[i] <= 'z' )
		{
			str[i] -= 32;
		}
	}
}
//================================================================
bool PathHelper::FileExist( const std::string& path )
{
	std::fstream stream;
	stream.open( path.c_str() );
	if ( !stream ) return false;
	stream.close();
	return true;
}



}