#pragma once
//
#include "MemDefines.h"
#include "Allocator.h"
#include "MemBank_Pooled_Dynamic.h"
//
namespace Memory
{
	class AllocatorCustom: public Allocator
	{
	public:
					AllocatorCustom( UnitCount count );
					~AllocatorCustom();
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
		MemBank_Pooled_Dynamic		m_allocator;
		std::ostream*				m_stream;
	};
}