#include <iostream>
//
#include "Pool_Dynamic.h"
#include "MemUtility.h"
#include "ReturnCodes.h"
//
namespace Memory
{
	template< unsigned Size >
	Pool_Dynamic<Size>::Pool_Dynamic()
		:m_unitSize( 0 ),
		m_allocs( 0 ),
		m_max_allocs( 0 ),
		m_nextFree( 0 ),
		m_max_units( 0 ),
		m_freeCount( 0 ),
		m_start( 0 ),
		m_end( 0 ),
		m_max_free( 0 ),
		m_freeSize( Size )
	{
		//
	}
	//
	template< unsigned Size >
	Pool_Dynamic<Size>::~Pool_Dynamic()
	{
		m_memory.Release();
		//
		m_unitSize		= 0;
		m_allocs		= 0;
		m_max_allocs	= 0;
		m_nextFree		= 0;
		m_freeCount		= 0;
	}
	//
	template< unsigned Size >
	int Pool_Dynamic<Size>::Init( UnitAllocSize size, UnitCount max )
	{
		m_memory.Acquire( size * max );
		//
		m_unitSize		= size;
		m_nextFree		= m_memory.GetMemory();
		m_max_units		= m_memory.Size() / size;
		m_start			= (Address) m_nextFree;
		m_end			= m_start + m_max_units*m_unitSize;
		//
		//std::cout<<"unit size: "<<size<<", Max: "<<m_max_units<<"\n";
		//
		return RET_SUCCESS;
	}
	//
	template< unsigned Size >
	Pointer Pool_Dynamic<Size>::Allocate()
	{
		if( m_allocs < m_max_units )
		{
			++m_allocs;
			//
#ifdef MEM_COLLECT_STATS
			if( m_max_allocs < m_allocs )
			{
				m_max_allocs = m_allocs;
			}
#endif
			//
			if( m_freeCount > 0 )
			{
				--m_freeCount;
				return m_freeList[m_freeCount];
			}
			else
			{
				//std::cout<<"Curr: "<<m_nextFree;
				Pointer p = m_nextFree;
				Address a = ((Address)m_nextFree) + m_unitSize;
				m_nextFree = (Pointer) a;
				//std::cout<<", Next: "<<m_nextFree<<"\n";
				return p;
			}
		}
		else
		{
			throw Not_Enough_Memory( m_unitSize, m_max_units );
		}
		return MEM_NULL_ADDR;
	}
	//
	template< unsigned Size >
	int Pool_Dynamic<Size>::Deallocate( Pointer mem )
	{
		Address a1 = (Address) mem;
		if( a1 < m_start || a1 > m_end )
		{
			//std::cout<<"Trying to free mem not allocated by this pool\n";
			return RET_ERR_MEM_PTR_OUTSIDE_KNOWN_RANGE;
		}
		//
		a1 -= m_start;
		if( a1%m_unitSize != 0 )
		{
			std::cout<<"unknown pointer!\n";
			return RET_ERR_MEM_PTR_UNKNOWN;
		}
		//
		/*
		for( unsigned i = 0; i < m_freeCount; ++i )
		{
			if( m_freeList[i] == mem )
			{
				std::cout<<"Memory is freed more than once: "<<mem<<"\n";
				return RET_ERR_MEM_PTR_FREED_MORE_THAN_ONCE;
			}
		}
		//
		if( m_freeCount < m_freeSize )
		{
			m_freeList[m_freeCount] = mem;
			++m_freeCount;
			--m_allocs;
			//
#ifdef MEM_COLLECT_STATS
			if( m_max_free < m_freeCount )
			{
				m_max_free = m_freeCount;
			}
#endif
			//
			//std::cout<<"deallocated: "<<mem<<"\n";
			return RET_SUCCESS;
		}
		else
		{
			std::cout<<"Pool_Dynamic:Cant dealloc, Free is full: "<<m_unitSize<<"\n";
		}
		*/
		Address a = (Address)m_nextFree;
		Address b = (Address)mem;
		if( b < a )
		{
			m_nextFree = mem;
		}
		--m_allocs;
		//
		return RET_SUCCESS;
	}
	//
	template< unsigned Size >
	UnitAllocSize Pool_Dynamic<Size>::UnitSize() const
	{
		return m_unitSize;
	}
	//
	template< unsigned Size >
	size_t Pool_Dynamic<Size>::SizeTotal() const
	{
		return m_unitSize*m_max_units;
	}
	//
	template< unsigned Size >
	size_t Pool_Dynamic<Size>::SizeFree() const
	{
		return SizeTotal() - SizeUsed();
	}
	//
	template< unsigned Size >
	size_t Pool_Dynamic<Size>::SizeUsed() const
	{
		return m_unitSize*m_allocs;
	}
	//
	template< unsigned Size >
	unsigned Pool_Dynamic<Size>::MaxAllocs() const
	{
		return m_max_allocs;
	}
	//
	template< unsigned Size >
	unsigned Pool_Dynamic<Size>::MaxFrees() const
	{
		return m_max_free;
	}
	//
	template< unsigned Size >
	UnitCount Pool_Dynamic<Size>::MaxUnits() const
	{
		return m_max_units;
	}
	//
	template< unsigned Size >
	PoolStatus Pool_Dynamic<Size>::Status() const
	{
		return (m_allocs < m_max_units) ? MEM_ACTIVE : MEM_FULL;
	}
	//
	template< unsigned Size >
	int Pool_Dynamic<Size>::Reset()
	{
		m_nextFree		= m_memory.GetMemory();
		m_allocs		= 0;
		m_freeCount		= 0;
		//
		return RET_SUCCESS;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	
	template<unsigned Size>
	std::ostream& operator<<( std::ostream& os, const Pool_Dynamic<Size>& pool )
	{
		os<<"Block Size: "<<pool.UnitSize()<<", Max possible allocs: "<<pool.MaxUnits()<<", Max used Allocs: "<<pool.MaxAllocs()<<", Max Frees: "<<pool.MaxFrees()<<"\n";
		return os;
	}
	
}