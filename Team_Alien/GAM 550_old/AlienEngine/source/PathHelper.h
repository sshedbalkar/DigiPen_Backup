#pragma once

namespace Framework
{

class PathHelper
{
public:
	static void PathGetExtension( const std::string& path, std::string& ext );
	static void PathGetFilename( const std::string& path, std::string& filename );
	static void PathChangeExtension( std::string& path, const std::string& ext );
	static void StringToUpper( std::string& str );
	static bool FileExist( const std::string& path );
};

}