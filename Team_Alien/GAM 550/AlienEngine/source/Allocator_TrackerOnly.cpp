#include "Allocator_TrackerOnly.h"
#include "MemUtility.h"
#include "ReturnCodes.h"
//
namespace Memory
{
	Allocator_TrackerOnly::Allocator_TrackerOnly()
	{
		InitDebug();
	}
	//
	Allocator_TrackerOnly::~Allocator_TrackerOnly()
	{
		Sweep();
	}
	//
	int Allocator_TrackerOnly::Sweep()
	{
		Allocator::Sweep();
		m_tracker.Sweep();
		return RET_SUCCESS;
	}
	//
	int Allocator_TrackerOnly::Init( UnitAllocSize size, UnitCount max )
	{
		return Allocator::Init( size, max );
	}
	//
	int Allocator_TrackerOnly::Status()
	{
		return MEM_ACTIVE;
	}
	//
	Pointer Allocator_TrackerOnly::Allocate( size_t size, AllocType type )
	{
		Pointer mem = Allocator::Allocate( size );
		m_tracker.Track( size, mem, type );
		return mem;
	}
	//
	int Allocator_TrackerOnly::Deallocate( Pointer mem, AllocType type )
	{
		m_tracker.Untrack( mem, type );
		return Allocator::Deallocate( mem );
	}
	//
	void Allocator_TrackerOnly::PrintLeaks( std::ostream& os )
	{
		os<<"\n--------------Tracker stats--------------\n";
		os<<m_tracker;
		os.flush();
	}
	//
	void Allocator_TrackerOnly::PrintStats( std::ostream& os )
	{
		//
	}
	//
	int Allocator_TrackerOnly::Reset()
	{
		return m_tracker.Reset();
	}
	//
	void Allocator_TrackerOnly::SetOStream( std::ostream& os )
	{
		m_stream = &os;
	}
}