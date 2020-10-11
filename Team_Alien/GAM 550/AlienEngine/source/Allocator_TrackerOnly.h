#pragma once
//
#include "MemDefines.h"
#include "Allocator.h"
#include "Tracker.h"
//
namespace Memory
{
	class Allocator_TrackerOnly: public Allocator
	{
	public:
					Allocator_TrackerOnly();
					~Allocator_TrackerOnly();
		//
	public:
		int			Init( UnitAllocSize size, UnitCount max );
		int			Status();
		int			Sweep();
		Pointer		Allocate( size_t size, AllocType type = ALLOC_SINGLE );
		int			Deallocate( Pointer mem, AllocType type = ALLOC_SINGLE );
		void		PrintStats( std::ostream& os );
		void		PrintLeaks( std::ostream& os );
		int			Reset();
		void		SetOStream( std::ostream& os );
		//
	private:
		Tracker				m_tracker;
		std::ostream*		m_stream;
	};
}