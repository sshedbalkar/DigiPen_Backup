#include <iostream>
#include <Windows.h>
#include <DbgHelp.h>
#include <stdio.h>
//
#include "Tracker.h"
#include "MemUtility.h"
#include "ReturnCodes.h"
//
#pragma comment ( lib, "dbghelp.lib" )
//
namespace Memory
{
	Node::Node()
		:Caller( 0 ),
		Alloc( 0 ),
		AllocId( 0 ),
		Size( 0 )
	{
		//
	}
	Node::~Node()
	{
		//
	}
	///////////////////////////////////////////////////////////////////////
	Tracker::Tracker()
		:m_count( 0 )
	{
		//
	}
	//
	Tracker::~Tracker()
	{
		//
	}
	//
	int Tracker::Track( size_t size, Pointer mem, AllocType type )
	{
		Node n;
		Address a		= GetCallingFunction( 6 );
		n.Caller		= a;
		n.Alloc			= mem;
		n.AllocId		= ++m_count;
		n.Size			= size;
		n.Type			= type;
		//
		m_allocs[mem]	= n;
		//
		return RET_SUCCESS;
	}
	//
	int Tracker::Untrack( Pointer mem, AllocType type )
	{
		if( mem == 0 ) return 0;
		AllocMapType::iterator it = m_allocs.find( mem );
		if( it != m_allocs.end() )
		{
			m_allocs.erase( it );
			return RET_SUCCESS;
		}
		ModuleInfo info;
		Address a		= GetCallingFunction( 6 );
		std::cout<<"Untrack: no addr: "<<mem;
		if( GetModuleInfoFromAddr(a, info) )
		{
			std::cout<<": "<<info.file<<":("<<info.line<<")\n";
		}
		else
		{
			std::cout<<"\n";
		}
		std::cin.get();
		return RET_ERR_TRACKR_CANT_FIND_DEALLOC_PTR;
	}
	//
	int Tracker::Sweep()
	{
		m_allocs.clear();
		return RET_SUCCESS;
	}
	//
	int Tracker::Reset()
	{
		return Sweep();
	}
	/////////////////////////////////////////////////////////////////////
	std::ostream& operator<<( std::ostream& os, const Tracker& tracker )
	{
		ModuleInfo info;
		os<<"Total Allocs: "<<tracker.m_count<<", Total Leaks: "<<tracker.m_allocs.size()<<"\n";
		for( Tracker::AllocMapType::const_iterator it1 = tracker.m_allocs.begin(), it2 = tracker.m_allocs.end(); it1 != it2; ++it1 )
		{
			const Node& n		= it1->second;
			if( GetModuleInfoFromAddr(n.Caller, info) )
			{
				os<<info.file<<":("<<info.line<<"): "<<n.Size<<" Bytes\n";
			}
		}
		return os;
	}
}