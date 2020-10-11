#pragma once
//
#include "MemDefines.h"
#include "Pool.h"
#include "Array.h"
#include "RawMemory.h"
//
namespace Memory
{
	template< unsigned Size >
	class Pool_Dynamic: public Pool
	{
	public:
		Pool_Dynamic();
		~Pool_Dynamic();
		//
		template<unsigned sz>
		friend std::ostream& operator<<( std::ostream& os, const Pool_Dynamic<sz>& pool );
		//
	public:
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
		utility::Array<Memory::Pointer, Size>	m_freeList;
		//
		UnitAllocSize	m_unitSize;
		unsigned		m_allocs;
		unsigned		m_max_allocs;
		Pointer			m_nextFree;
		UnitCount		m_max_units;
		RawMemory		m_memory;
		unsigned		m_freeCount;
		unsigned		m_max_free;
		Address			m_start;
		Address			m_end;
		unsigned		m_freeSize;
	};
}
//
#include "Pool_Dynamic.cpp"