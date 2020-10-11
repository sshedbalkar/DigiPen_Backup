#pragma once
//
#include "Allocator.h"
#include "MemBank_Pooled.h"
//
namespace Memory
{
	class AllocatorDebug: public Allocator
	{
	public:
					AllocatorDebug();
					~AllocatorDebug();
		//
	public:
		int			Init( UnitAllocSize size, UnitCount max );
		int			Status();
		int			Sweep();
		Pointer		Allocate( size_t size, AllocType type = ALLOC_SINGLE );
		int			Deallocate( Pointer mem, AllocType type = ALLOC_SINGLE );
		void		PrintLeaks( std::ostream& os );
		void		PrintStats( std::ostream& os );
		int			Reset();
		void		SetOStream( std::ostream& os );
		//
	private:
		MemBank_Pooled	m_allocator;
		std::ostream*	m_stream;
	};
}