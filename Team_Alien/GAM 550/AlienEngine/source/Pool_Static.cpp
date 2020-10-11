#include <iostream>
//
#include "Pool_Static.h"
#include "MemUtility.h"
#include "ReturnCodes.h"
#include "Global.h"
//
namespace Memory
{
	Pool_Static::Pool_Static()
		:m_unitSize( 0 ),
		m_allocs( 0 ),
		m_max_allocs( 0 ),
		m_nextFree( 0 ),
		m_max_units( 0 ),
		m_freeCount( 0 ),
		m_start( 0 ),
		m_end( 0 ),
		m_max_free( 0 )
	{
		//
	}
	//
	Pool_Static::~Pool_Static()
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
	int Pool_Static::Init( UnitAllocSize size, UnitCount max )
	{
		m_memory.Acquire( UpperPowerOfTwo(size * max) );
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
	Pointer Pool_Static::Allocate()
	{
		Pointer p = MEM_NULL_ADDR;
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
				p = m_freeList[m_freeCount];
			}
			else
			{
				//if( !GAME_EXIT )
				//{
					//std::cout<<"Curr: "<<m_nextFree;
					p = m_nextFree;
					Address a = ((Address)m_nextFree) + m_unitSize;
					m_nextFree = (Pointer) a;
					//std::cout<<", Next: "<<m_nextFree<<"\n";
				//}
				//else
				//{
					//p = ::malloc( m_unitSize );
				//}
			}
		}
		else
		{
			throw Not_Enough_Memory( m_unitSize, m_max_units );
		}
		return p;
	}
	//
	int Pool_Static::Deallocate( Pointer mem )
	{
		//if( !GAME_EXIT )
		//{
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
			for( unsigned i = 0; i < m_freeCount; ++i )
			{
				if( m_freeList[i] == mem )
				{
					std::cout<<"Memory is freed more than once: "<<mem<<"\n";
					return RET_ERR_MEM_PTR_FREED_MORE_THAN_ONCE;
				}
			}
			//
			if( m_freeCount < FREE_LIST_SIZE )
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
				//std::cout<<"Cant dealloc, Free is full: "<<m_unitSize<<"\n";
			}
		//}
		//else
		//{
			//::free( mem );
			//return RET_SUCCESS;
		//}
		//
		return RET_FAIL;
	}
	//
	UnitAllocSize Pool_Static::UnitSize() const
	{
		return m_unitSize;
	}
	//
	size_t Pool_Static::SizeTotal() const
	{
		return m_unitSize*m_max_units;
	}
	//
	size_t Pool_Static::SizeFree() const
	{
		return SizeTotal() - SizeUsed();
	}
	//
	size_t Pool_Static::SizeUsed() const
	{
		return m_unitSize*m_allocs;
	}
	//
	unsigned Pool_Static::MaxAllocs() const
	{
		return m_max_allocs;
	}
	//
	unsigned Pool_Static::MaxFrees() const
	{
		return m_max_free;
	}
	//
	UnitCount Pool_Static::MaxUnits() const
	{
		return m_max_units;
	}
	//
	PoolStatus Pool_Static::Status() const
	{
		return (m_allocs < m_max_units) ? MEM_ACTIVE : MEM_FULL;
	}
	//
	int Pool_Static::Reset()
	{
		m_nextFree		= m_memory.GetMemory();
		m_allocs		= 0;
		m_freeCount		= 0;
		//
		return RET_SUCCESS;
	}
	///////////////////////////////////////////////////////////////////////////
	std::ostream& operator<<( std::ostream& os, const Pool_Static& pool )
	{
		os<<"Block Size: "<<pool.UnitSize()<<", Max possible allocs: "<<pool.MaxUnits()<<", Max used Allocs: "<<pool.MaxAllocs()<<", Max Frees: "<<pool.MaxFrees()<<"\n";
		return os;
	}
}