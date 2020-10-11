#pragma once
//
#include <ostream>
//
#include "MemBank.h"
#include "MemDefines.h"
#include "Pool.h"
//
namespace Memory
{
	struct PoolCount
	{
		UnitAllocSize	size;
		unsigned		count;
	};
	//
	class MemBank_Pooled_Dynamic: public MemBank
	{
	public:
		MemBank_Pooled_Dynamic( UnitCount unitCount );
		~MemBank_Pooled_Dynamic();
		friend std::ostream& operator<<( std::ostream& os, const MemBank_Pooled_Dynamic& bank );
		//
	public:
		Pointer			Allocate( size_t size );
		int				Deallocate( Pointer ptr );
		int				Sweep();
		size_t			SizeTotal() const;
		size_t			SizeFree() const;
		size_t			SizeUsed() const;
		int				Reset();
		void			SetOStream( std::ostream& os );
		//
	public:
		int				Init();
		//
	private:
		Pool*			AllocNewPool( size_t size, UnitCount count );
		Pool*			GetPoolBySize( UnitAllocSize size );
		//
	private:
		Pool*			m_pools[MAX_POOLS];
		UnitAllocSize	m_minSize;
		UnitAllocSize	m_maxSize;
		unsigned		m_count;
		UnitCount		m_unitCount;
		Pool*			m_lastUsed;
		unsigned		m_poolTypesCount;
		PoolCount		m_poolTypes[MAX_POOLS];
		std::ostream*	m_stream;
	};
}