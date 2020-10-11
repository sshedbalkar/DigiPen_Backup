#include <Windows.h>
#include <DbgHelp.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
//
#include "MemUtility.h"
#include "ReturnCodes.h"
//
#pragma comment ( lib, "dbghelp.lib" )
//
namespace Memory
{
	Pointer hProcess	= 0;
	Pointer hThread		= 0;
	bool	l_debugInited	= false;
	//
	Not_Enough_Memory::Not_Enough_Memory( UnitAllocSize size, UnitCount max )
		:UnitSize( size),
		MaxUnits( max )
	{
		::sprintf_s( Msg, 128, "Cannot allocate memory of size: %d, Max units: %d", UnitSize, MaxUnits );
	}
	//
	const char* Not_Enough_Memory::what() const
	{
		return Msg;
	}
	//////////////////////////////////////////////////////
	bool InitDebug()
	{
		std::cout<<"InitDebug\n";
		if( !l_debugInited )
		{
			hProcess	= ::GetCurrentProcess();
			hThread		= ::GetCurrentThread();
			bool initRes = !!::SymInitialize( hProcess, NULL, true );
			::SymSetOptions( ::SymGetOptions() | SYMOPT_LOAD_LINES);
			if( !initRes )
			{
				DWORD err = ::GetLastError();
				std::cout<<"SymInitialize failed with error: "<<err<<"\n";;
			}
			l_debugInited = true;
			return initRes;
		}
		return l_debugInited;
	}
	void Debugprintf( const char *str, ... )
	{
		va_list mkr;
		char buff[2048];
		//
		va_start( mkr, str );
		vsprintf_s( buff, _countof(buff), str, mkr );
		va_end( mkr );
		//
		::OutputDebugStringA( buff );
		std::cout<<buff;
	}
	//
	void PrintAddress( Address address )
	{
		ModuleInfo info;
		if( GetModuleInfoFromAddr(address, info) )
		{
			std::cout<<info.file<<": ("<<info.line<<")\n";
			//Debugprintf( "%s(%u): ", info.file, info.line );
		}
	}
	//
	void ShowAddressInDebugger( Address address )
	{
		ModuleInfo info;
		if( GetModuleInfoFromAddr(address, info) )
		{
			Debugprintf( "%s(%u)", info.file, info.line );
		}
	}
	//
	Address GetCallingFunction( unsigned skipDepth )
	{
		CONTEXT c;
	#ifdef _WIN64
		::RtlCaptureContext( &c );
	#else
		c.ContextFlags = CONTEXT_CONTROL;
		__asm
		{
	LABEL:  mov eax, [LABEL];
			mov c.Eip, eax;
			mov c.Ebp, ebp;
			mov c.Esp, esp;
		}
	#endif
		STACKFRAME64 stack_frame		= { 0 };
		stack_frame.AddrPC.Mode			= AddrModeFlat;
		stack_frame.AddrPC.Offset		= c.Eip;
		stack_frame.AddrStack.Mode		= AddrModeFlat;
		stack_frame.AddrStack.Offset	= c.Esp;
		stack_frame.AddrFrame.Mode		= AddrModeFlat;
		stack_frame.AddrFrame.Offset	= c.Ebp;
		//
		for( unsigned i = 0; i<skipDepth; ++i )
		{
			::StackWalk64( IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack_frame, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL );
		}
		//
		return stack_frame.AddrPC.Offset;
	}
	//
	CallStackType& GetCallStack( unsigned skipDepth, unsigned maxDepth, CallStackType& stack )
	{
		CONTEXT c;
	#ifdef _WIN64
		::RtlCaptureContext( &c );
	#else
		c.ContextFlags = CONTEXT_CONTROL;
		__asm
		{
	LABEL:  mov eax, [LABEL];
			mov c.Eip, eax;
			mov c.Ebp, ebp;
			mov c.Esp, esp;
		}
	#endif
		STACKFRAME64 stack_frame		= { 0 };
		stack_frame.AddrPC.Mode			= AddrModeFlat;
		stack_frame.AddrPC.Offset		= c.Eip;
		stack_frame.AddrStack.Mode		= AddrModeFlat;
		stack_frame.AddrStack.Offset	= c.Esp;
		stack_frame.AddrFrame.Mode		= AddrModeFlat;
		stack_frame.AddrFrame.Offset	= c.Ebp;
		//
		for( unsigned i = 0; i<skipDepth; ++i )
		{
			::StackWalk64( IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack_frame, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL );
		}
		//
		for( unsigned i = 0; i < CALLSTACK_SIZE; ++i )
		{
			stack[i] = 0;
		}
		stack[0] = stack_frame.AddrPC.Offset;
		//stack.clear();
		//stack.push_back( stack_frame.AddrPC.Offset );
		for( unsigned i = 1; i<maxDepth; ++i )
		{
			::StackWalk64( IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack_frame, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL );
			//stack.push_back( stack_frame.AddrPC.Offset );
			stack[i] = stack_frame.AddrPC.Offset;
		}
		return stack;
	}
	//
	Address GetNearestModule( unsigned skipDepth )
	{
		CONTEXT c;
	#ifdef _WIN64
		::RtlCaptureContext( &c );
	#else
		c.ContextFlags = CONTEXT_CONTROL;
		__asm
		{
	LABEL:  mov eax, [LABEL];
			mov c.Eip, eax;
			mov c.Ebp, ebp;
			mov c.Esp, esp;
		}
	#endif
		STACKFRAME64 stack_frame		= { 0 };
		stack_frame.AddrPC.Mode			= AddrModeFlat;
		stack_frame.AddrPC.Offset		= c.Eip;
		stack_frame.AddrStack.Mode		= AddrModeFlat;
		stack_frame.AddrStack.Offset	= c.Esp;
		stack_frame.AddrFrame.Mode		= AddrModeFlat;
		stack_frame.AddrFrame.Offset	= c.Ebp;
		//
		bool ret;
		for( unsigned i = 0; i < skipDepth; ++i )
		{
			ret = !!::StackWalk64( IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack_frame, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL );
		}
		//
		IMAGEHLP_LINE64 hlp = { 0 };
		DWORD symbol_offset = 0;
		Address address;
		DWORD error;
		while( true )
		{
			address = stack_frame.AddrPC.Offset;
			symbol_offset = 0;
			if( ::SymGetLineFromAddr64(hProcess, address, &symbol_offset, &hlp) )
			{
				int l = strlen( hlp.FileName );
				const char* extensionName = hlp.FileName + l - 3;
				
				if( !::strcmp( extensionName, "cpp" ) )
				{
					return address;
				}
			}
			else
			{
				error = GetLastError();
				std::cout<<"Error: "<<error<<"\n";
			}
			ret = !!StackWalk64( IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stack_frame, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL );
		}
		return stack_frame.AddrPC.Offset;
	}
	//
	const unsigned len = 256;
	char buff[len];
	Cstr MemSizeToText( size_t size )
	{
		double s = size;
		if( size < MEM_SIZE_1_KB )
		{
			::sprintf_s( buff, len, "%d Bytes", size );
		}
		else if( size < MEM_SIZE_1_MB )
		{
			size /= MEM_SIZE_1_KB;
			::sprintf_s( buff, len, "%d KB", size );
		}
		else if( size < MEM_SIZE_1_GB )
		{
			s /= MEM_SIZE_1_MBf;
			::sprintf_s( buff, len, "%4.4f MB", s );
		}
		else
		{
			s /= MEM_SIZE_1_GBf;
			::sprintf_s( buff, len, "%6.6f GB", s );
		}
		//
		return buff;
	}
	//
	unsigned UpperPowerOfTwo( unsigned v )
	{
		--v;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		++v;
		return v;
	}
	//
	bool GetModuleInfoFromAddr( Address address, ModuleInfo& info )
	{
		IMAGEHLP_LINE64 hlp = { 0 };
		DWORD symbol_offset = 0;
		if( ::SymGetLineFromAddr64(hProcess, address, &symbol_offset, &hlp) )
		{
			::strcpy_s( info.file, hlp.FileName );
			info.line	= hlp.LineNumber;
			return true;
		}
		return false;
	}
	//
	unsigned Round( float number )
	{
		return unsigned(number < 0.0f ? ::ceil( number - 0.5f ) : ::floor( number + 0.5f ));
	}
	//
	void PrintCallStack( const Memory::CallStackType& stack, std::ostream& os )
	{
		Memory::ModuleInfo info;
		unsigned i = 0;
		//for( Memory::CallStackType::const_iterator it1 = stack.begin(), it2 = stack.end(); it1 != it2; ++it1 )
		while( stack[i] != 0 )
		{
			if( Memory::GetModuleInfoFromAddr(stack[i], info) )
			{
				os<<info.file<<": ("<<info.line<<")\n";
			}
			else
			{
				os<<"unknown:(0)\n";
			}
			++i;
		}
	}
	//
	std::ostream& operator<<( std::ostream& os, const Memory::CallStackType& stack )
	{
		PrintCallStack( stack, os );
		return os;
	}
}