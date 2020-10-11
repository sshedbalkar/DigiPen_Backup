#pragma once
//
#include <ostream>
//
#include "MemDefines.h"
#include "RawMemory.h"
#include "Pool.h"
//
namespace Memory
{
	class MemBank_Pool_Staticed;
	//
	class Pool_Static: public Memory::Pool
	{
	private:
		Pool_Static();
		virtual ~Pool_Static();
		friend class MemBank_Pooled;
		friend std::ostream& operator<<( std::ostream& os, const Pool_Static& pool );
		//
	public:
		static const int	FREE_LIST_SIZE = 262144;
		//
		int				Init( UnitAllocSize size, UnitCount max );
		Pointer			Allocate();
		int				Deallocate( Pointer mem );
		UnitAllocSize	UnitSize() const;
		size_t			SizeTotal() const;
		size_t			SizeFree() const;
		size_t			SizeUsed() const;
		unsigned		MaxAllocs() const;
		unsigned		MaxFrees() const;
		UnitCount		MaxUnits() const;
		PoolStatus		Status() const;
		int				Reset();
		//
	private:
		UnitAllocSize	m_unitSize;
		unsigned		m_allocs;
		unsigned		m_max_allocs;
		Pointer			m_nextFree;
		UnitCount		m_max_units;
		RawMemory		m_memory;
		Memory::Pointer	m_freeList[FREE_LIST_SIZE];
		unsigned		m_freeCount;
		unsigned		m_max_free;
		Address			m_start;
		Address			m_end;
	};
}