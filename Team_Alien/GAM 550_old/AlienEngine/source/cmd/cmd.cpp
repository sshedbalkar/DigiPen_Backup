#include <stdio.h>
#include <sstream>
#include <string>
#include <map>
#include <Windows.h>
#include "cmd.h"

namespace Framework
{
#define MAX_STRING_LENGTH	20

void test( ParamData* param )
{
	const char* s = param->ParamString();
	float f = param->ParamFloat();
	int i = param->ParamInt();

	printf("cb %s %0.2f %i\n", s, f, i );
}

//========================================
Cmd::Cmd() : enable(true)
{
//	RegisterFunc( "test", test, "sfi" );
}
//========================================
void Cmd::CharacterInput( char c )
{
//	printf("%d\n", c );
	switch ( c )
	{
	case 0: // up
		break;
	case 8:	// backspace
		break;
	case 13: // return
		ProcessCmd();
		prev_cmd = current_cmd;
		current_cmd.clear();
		break;
	case 32: // space
		current_cmd.push_back( 32 );
		break;
	default:
		if ( c >= '!' && c <= '~' ) current_cmd.push_back( c );
		break;
	}
}
//========================================
void Cmd::RegisterFunc( const char* name, CmdFunction func, const char* format )
{
	functions[name].func = func;
	functions[name].format = format;
}
//========================================
void Cmd::ProcessCmd()
{
	std::stringstream ss(current_cmd);
	std::string funcid;

	ss >>funcid;
	if ( functions.find(funcid) != functions.end() )
	{
		CallbackFunc* cb = &functions[funcid];
		ParamData param;
		if ( param.ParseParam( ss, cb->format.c_str() ) )
			cb->func( &param );
		else printf( ">##command line: wrong parameter format.\n");
	}
	else 
		printf("no function %s is registered.\n", funcid.c_str() );
}
//========================================
ParamData::ParamData() : data(NULL), ptr(NULL)
{
}
//========================================
ParamData::~ParamData()
{
	if ( data ) delete [] data; data = NULL;
	ptr = NULL;
}
//========================================
bool ParamData::ParseParam( std::stringstream& str, const char* format )
{
	const char* fmt = format;
	float f; 
	int i;
	int size = 0;
	std::string s;

	while( *fmt != 0 )
	{
		if ( *fmt == 'f' || *fmt == 'i' ) size += 4;
		else if ( *fmt == 's' ) size += MAX_STRING_LENGTH;
		++fmt;
	}
	data = new char[size];
	ZeroMemory( data, size );
	ptr = data;
	fmt = format;
	while( *fmt != 0 )
	{
		switch ( *fmt )
		{
		case 'f':
			if ( !( str >>f ) ) return false;
			*(float*)ptr = f;
			ptr += 4;
			printf("%0.2f\n", f );
			break;
		case 'i':
			if ( !( str >>i ) ) return false;
			*(int*)ptr = i;
			ptr += 4;
			printf("%d\n", i );
			break;
		case 's':
			if ( !(str >>s ) ) return false;
			if ( s.size() >= MAX_STRING_LENGTH - 1 ) return false;
			memcpy( ptr, s.c_str(), s.size() );
			ptr += MAX_STRING_LENGTH;
			printf("%s\n", s.c_str() );
			break;
		}
		++fmt;
	}
	ptr = data;
	return true;
}
//========================================
int ParamData::ParamInt()
{
	int ret = *(int*)ptr;
	ptr += 4;
	return ret;
}
//========================================
float ParamData::ParamFloat()
{
	float ret = *(float*)ptr;
	ptr += 4;
	return ret;
}
//========================================
const char* ParamData::ParamString()
{
	const char* ret = ptr;
	ptr += MAX_STRING_LENGTH;
	return ret;
}
}