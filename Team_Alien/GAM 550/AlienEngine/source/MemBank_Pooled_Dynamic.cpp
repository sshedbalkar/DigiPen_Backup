#include <iostream>
//
#include "MemBank_Pooled_Dynamic.h"
#include "Pool_Dynamic.h"
#include "MemUtility.h"
#include "ReturnCodes.h"
//
namespace Memory
{
	MemBank_Pooled_Dynamic::MemBank_Pooled_Dynamic( UnitCount count )
		:m_unitCount( count ),
		m_minSize( UINT_MAX ),
		m_maxSize( 0 ),
		m_count( 0 ),
		m_lastUsed( 0 ),
		m_poolTypesCount( 0 )
	{
		InitDebug();
	}
	//
	MemBank_Pooled_Dynamic::~MemBank_Pooled_Dynamic()
	{
		for( unsigned i = 0; i < m_count; ++i )
		{
			delete m_pools[i];
		}
	}
	//
	int MemBank_Pooled_Dynamic::Init()
	{
		return 0;
	}
	//
	Pointer MemBank_Pooled_Dynamic::Allocate( size_t size )
	{
		Pool* p = 0;
		if( size < m_minSize || size > m_maxSize )
		{
			p = AllocNewPool( size, m_unitCount );
			m_poolTypes[m_poolTypesCount].count		= 1;
			m_poolTypes[m_poolTypesCount].size		= size;
			++m_poolTypesCount;
		}
		else
		{
			if( !(p = GetPoolBySize(size)) )
			{
				unsigned pw = 1;
				for( unsigned i = 0; i < m_poolTypesCount; ++i )
				{
					if( m_poolTypes[i].size == size )
					{
						pw <<= m_poolTypes[i].count;
						++m_poolTypes[i].count;
						break;
					}
				}
				//
				p = AllocNewPool( size, m_unitCount*pw );
			}
		}
		//
		m_lastUsed = p;
		return p->Allocate();
	}
	//
	int MemBank_Pooled_Dynamic::Deallocate( Pointer ptr )
	{
		if( ptr == 0 ) return RET_SUCCESS;
		//
		unsigned i;
		for( i = 0; i < m_count && m_pools[i]->Deallocate( ptr ); ++i )
		{
			//
		}
		//
		if( i == m_count )
		{
			std::cout<<"Dynamic::Trying to Free invalid ptr: "<<ptr<<"\n";
			CallStackType stack;
			std::cout<<GetCallStack( 4, 12, stack );
			std::cin.get();
			return RET_ERR_MEM_TRYING_TO_FREE_INVLD_PTR;
		}
		//
		return RET_SUCCESS;
	}
	//
	Pool* MemBank_Pooled_Dynamic::AllocNewPool( size_t size, UnitCount count )
	{
		//std::cout<<"Creating new poooooooooooooooool: "<<size<<", "<<count<<"\n";
		Pool* p1;
		UnitAllocSize sz;
		//
		p1 = new Pool_Dynamic<MEM_POOL_DYNAMIC_FREE_LIST_SIZE>();
		p1->Init( size, count );
		//
		m_pools[m_count] = p1;
		++m_count;
		//
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz = m_pools[i]->UnitSize();
			if( sz < m_minSize )
			{
				m_minSize = sz;
			}
			if( sz > m_maxSize )
			{
				m_maxSize = sz;
			}
		}
		//
		return p1;
	}
	//
	Pool* MemBank_Pooled_Dynamic::GetPoolBySize( UnitAllocSize size )
	{
		if( m_lastUsed->UnitSize() == size && m_lastUsed->Status() == MEM_ACTIVE )
		{
			return m_lastUsed;
		}
		//
		Pool* p;
		for( unsigned i = 0; i < m_count; ++i )
		{
			p = m_pools[i];
			if( p->UnitSize() != size || p->Status() != MEM_ACTIVE ) continue;
			return p;
		}
		//
		return 0;
	}
	//
	int MemBank_Pooled_Dynamic::Sweep()
	{
		return RET_SUCCESS;
	}
	//
	size_t MemBank_Pooled_Dynamic::SizeTotal() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i]->SizeTotal();
		}
		//
		return sz;
	}
	//
	size_t MemBank_Pooled_Dynamic::SizeFree() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i]->SizeFree();
		}
		//
		return sz;
	}
	//
	size_t MemBank_Pooled_Dynamic::SizeUsed() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i]->MaxAllocs() * m_pools[i]->UnitSize();
		}
		//
		return sz;
	}
	//
	int MemBank_Pooled_Dynamic::Reset()
	{
		for( unsigned i = 0; i < m_count; ++i )
		{
			m_pools[i]->Reset();
		}
		//
		return 0;
	}
	//
	void MemBank_Pooled_Dynamic::SetOStream( std::ostream& os )
	{
		m_stream = &os;
	}
	//
	//////////////////////////////////////////////////////////////////////////////
	std::ostream& operator<<( std::ostream& os, const MemBank_Pooled_Dynamic& bank )
	{
		unsigned s1 = bank.SizeTotal();
		unsigned s2 = bank.SizeUsed();
		Pool_Dynamic<MEM_POOL_DYNAMIC_FREE_LIST_SIZE>* p;
		//
		os<<"Total size: "<<MemSizeToText(s1);
		os<<", Used: "<<MemSizeToText(s2)<<"\n";
		for( unsigned i = 0; i < bank.m_count; ++i )
		{
			p = static_cast<Pool_Dynamic<MEM_POOL_DYNAMIC_FREE_LIST_SIZE>*>(bank.m_pools[i]);
			os<<*p;
		}
		//
		return os;
	}
}