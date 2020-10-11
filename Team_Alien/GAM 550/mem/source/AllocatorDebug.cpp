#include "AllocatorDebug.h"
#include "MemUtility.h"
#include "ReturnCodes.h"

//
//Memory::Tracker				l_tracker;
//Memory::MemBank_Pooled		l_allocator_2;
//
namespace Memory
{
	AllocatorDebug::AllocatorDebug()
	{
		InitDebug();
	}
	//
	AllocatorDebug::~AllocatorDebug()
	{
		Sweep();
	}
	//
	int AllocatorDebug::Sweep()
	{
		m_allocator.Sweep();
		m_tracker.Sweep();
		return RET_SUCCESS;
	}
	//
	int AllocatorDebug::Init( UnitAllocSize size, UnitCount max )
	{
		m_allocator.Init();
		return RET_SUCCESS;
	}
	//
	int AllocatorDebug::Status()
	{
		return MEM_ACTIVE;
	}
	//
	Pointer AllocatorDebug::Allocate( size_t size, AllocType type )
	{
		try
		{
			Pointer mem = m_allocator.Allocate( size );
			m_tracker.Track( size, mem, type );
			return mem;
		}
		catch( Not_Enough_Memory& ex )
		{
			(*m_stream)<<ex.what();
			(*m_stream).flush();
			throw ex;
		}
	}
	//
	int AllocatorDebug::Deallocate( Pointer mem, AllocType type )
	{
		m_tracker.Untrack( mem, type );
		m_allocator.Deallocate( mem );
		return RET_SUCCESS;
	}
	//
	void AllocatorDebug::PrintLeaks( std::ostream& os )
	{
		os<<"\n--------------Tracker stats--------------\n";
		os<<m_tracker;
		os.flush();
	}
	//
	void AllocatorDebug::PrintStats( std::ostream& os )
	{
		os<<"\n--------------Allocator stats--------------\n";
		os<<m_allocator;
		os.flush();
	}
	//
	int AllocatorDebug::Reset()
	{
		m_tracker.Reset();
		return m_allocator.Reset();
	}
	//
	void AllocatorDebug::SetOStream( std::ostream& os )
	{
		m_stream = &os;
		m_allocator.SetOStream( os );
	}
}