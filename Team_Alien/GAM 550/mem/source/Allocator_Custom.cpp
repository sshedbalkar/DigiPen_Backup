#include <iostream>
//
#include "Allocator_Custom.h"
#include "MemUtility.h"
//
namespace Memory
{
	AllocatorCustom::AllocatorCustom( UnitCount count )
		:m_allocator( count ),
		m_stream( 0 )
	{
		//
	}
	//
	AllocatorCustom::~AllocatorCustom()
	{
		Sweep();
	}
	//
	int AllocatorCustom::Sweep()
	{
		return m_allocator.Sweep();
	}
	//
	int AllocatorCustom::Init( UnitAllocSize size, UnitCount max )
	{
		return m_allocator.Init();
	}
	//
	int AllocatorCustom::Status()
	{
		return MEM_ACTIVE;
	}
	//
	Pointer AllocatorCustom::Allocate( size_t size, AllocType type )
	{
		//std::cout<<"AllocatorCustom::Allocate: "<<size<<"\n";
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
	}
	//
	int AllocatorCustom::Deallocate( Pointer mem, AllocType type )
	{
		//std::cout<<"AllocatorCustom::Deallocate: "<<mem<<"\n";
		return m_allocator.Deallocate( mem );
	}
	//
	void AllocatorCustom::PrintLeaks( std::ostream& os )
	{
		//
	}
	//
	void AllocatorCustom::SetOStream( std::ostream& os )
	{
		m_stream = &os;
		m_allocator.SetOStream( os );
	}
	//
	void AllocatorCustom::PrintStats( std::ostream& os )
	{
		os<<"\n-----------Custom Allocator Stats-----------\n";
		os<<m_allocator;
		os.flush();
	}
	//
	int AllocatorCustom::Reset()
	{
		std::cout<<"AllocatorCustom::Reset\n";
		return m_allocator.Reset();
	}
}