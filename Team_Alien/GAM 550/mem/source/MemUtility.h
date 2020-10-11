#pragma once
//
//#include <string>
#include <ostream>
#include <vector>
#include <exception>
//
#include "Array.h"
#include "MemDefines.h"
//
namespace Memory
{
	//typedef std::vector<Address>	CallStackType;
	typedef utility::Array<Address, CALLSTACK_SIZE>	CallStackType;
	//
	class Not_Enough_Memory: public std::exception
	{
	public:
		Not_Enough_Memory( UnitAllocSize size, UnitCount max );
		const char* what() const;
		//
		UnitAllocSize	UnitSize;
		UnitCount		MaxUnits;
		char			Msg[128];
	};
	/////////////////////////////////////
	struct ModuleInfo
	{
		ModuleInfo()
			:line( 0 )
		{
			::memset( file, 0, MAX_FILE_NAME_LENGTH );
		}
		//
		~ModuleInfo(){}
		//
		char		file[MAX_FILE_NAME_LENGTH];
		unsigned	line;
	};
	//
	void				Debugprintf( const char *str, ... );
	void				PrintAddress( Address address );
	void				ShowAddressInDebugger( Address address );
	Address				GetCallingFunction( unsigned skipDepth );
	Address				GetNearestModule( unsigned skipDepth );
	Cstr				MemSizeToText( size_t size );
	unsigned			UpperPowerOfTwo( unsigned v );
	bool				InitDebug();
	bool				GetModuleInfoFromAddr( Address address, ModuleInfo& info );
	unsigned			Round( float num );
	CallStackType&		GetCallStack( unsigned skipDepth, unsigned maxDepth, CallStackType& stack );
	void				PrintCallStack( const Memory::CallStackType& stack, std::ostream& os );
	std::ostream&		operator<<( std::ostream& os, const Memory::CallStackType& stack );
	//
	extern Pointer		hProcess;
	extern Pointer		hThread;
}