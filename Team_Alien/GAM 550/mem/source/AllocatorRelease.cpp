#include <iostream>
//
#include "AllocatorRelease.h"
#include "MemUtility.h"
#include "ReturnCodes.h"
//
namespace Memory
{
	AllocatorRelease::AllocatorRelease()
	{
		//
	}
	//
	AllocatorRelease::~AllocatorRelease()
	{
		Sweep();
	}
	//
	int AllocatorRelease::Sweep()
	{
		return m_allocator.Sweep();
	}
	//
	int AllocatorRelease::Init( UnitAllocSize size, UnitCount max )
	{
		return m_allocator.Init();
		//return 0;
	}
	//
	int AllocatorRelease::Status()
	{
		return MEM_ACTIVE;
	}
	//
	Pointer AllocatorRelease::Allocate( size_t size, AllocType type )
	{
		
		try
		{
			return m_allocator.Allocate( size );
		}
		catch( Not_Enough_Memory& ex )
		{
			(*m_stream)<<ex.what();
			(*m_stream).flush();
			throw ex;
		}
		
		//return ::malloc( size );
	}
	//
	int AllocatorRelease::Deallocate( Pointer mem, AllocType type )
	{
		return m_allocator.Deallocate( mem );
		//::free( mem );
		//return 0;
	}
	//
	void AllocatorRelease::PrintStats( std::ostream& os )
	{
		os<<"\n--------------Allocator stats--------------\n";
		os<<m_allocator;
		os.flush();
	}
	//
	void AllocatorRelease::PrintLeaks( std::ostream& os )
	{
		//
	}
	//
	int AllocatorRelease::Reset()
	{
		return m_allocator.Reset();
	}
	//
	void AllocatorRelease::SetOStream( std::ostream& os )
	{
		m_stream = &os;
		m_allocator.SetOStream( os );
	}
}