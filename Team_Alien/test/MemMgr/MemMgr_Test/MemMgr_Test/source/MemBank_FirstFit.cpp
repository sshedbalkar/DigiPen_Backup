#include <iostream>
//
#include "MemBank_FirstFit.h"
#include "Block.h"
//
namespace Memory
{
	MemBank_FirstFit::MemBank_FirstFit()
		:m_used( 0 ),
		m_max_available_chunk_size( 0 )
	{
		//
	}
	//
	MemBank_FirstFit::~MemBank_FirstFit()
	{
		Sweep();
	}
	//
	Pointer MemBank_FirstFit::Allocate( size_t size )
	{
		std::cout<<"MemBank_FirstFit::Allocate: "<<size<<"\n";
		if( m_used < 1 || m_max_available_chunk_size < size )
		{
			if( m_used < MAX_MEM_PAGES_PER_BANK )
			{
				AllocateNewPage( m_used, MEM_PER_PAGE );
				++m_used;
				m_max_available_chunk_size = MEM_PER_PAGE;
			}
			else
			{
				throw No_Memory();
			}
		}
		//
		Pointer p = 0;
		size_t sz = size + Block::SizeBlock;
		Block *prev, *next;
		unsigned id = 0;
		//
		for( unsigned i = 0; i < m_used; ++i )
		{
			if( FindInPage(i, sz, p, prev, next) )
			{
				id = i;
				i = m_used;
			}
		}
		//
		if( p != 0 )
		{
			Block* bl		= InsertNewAlloc( p, id, sz, prev, next );
			Pointer ptr		= bl->SetPayload( size );
			//
			UpdateMaxChunkInPage( id );
			UpdateMaxChunk();
			//
			return ptr;
		}
		//
		return p;
	}
	//
	int MemBank_FirstFit::Deallocate( Pointer ptr )
	{
		std::cout<<"MemBank_FirstFit::Deallocate: "<<ptr<<"\n";
		Address left, right;
		Address addr	= (Address) ptr;
		Block* bl		= 0;
		unsigned id;
		//
		for( unsigned i = 0; i < m_used; ++i )
		{
			Page& page	= m_bank[i];
			left		= ( Address ) page.Start;
			right		= left + page.Size;
			if( (addr > left) && (addr <= right) )
			{
				bl	= GetBlockFromPointer( i, ptr );
				id	= i;
				i	= m_used;
			}
		}
		//
		if( bl != 0 )
		{
			DeleteAlloc( id, bl );
			UpdateMaxChunkInPage( id );
			UpdateMaxChunk();
		}
		else
		{
			return ERR_MEM_COULD_NOT_FIND_ADDR;
		}
		//
		return 0;
	}
	//
	void MemBank_FirstFit::DeleteAlloc( unsigned id, Block* bl )
	{
		Page& page		= m_bank[id];
		//
		if( bl->Prev != PAGE_END_BLOCK )
		{
			bl->Prev->Next = bl->Next;
		}
		if( bl->Next != PAGE_END_BLOCK )
		{
			bl->Next->Prev = bl->Prev;
		}
		page.TotalFree += bl->SizeTotal();
		if( --page.Allocations == 0 )
		{
			page.FirstAlloc = 0;
		}
		else if( bl == static_cast<Block*>(page.FirstAlloc) )
		{
			page.FirstAlloc = bl->Next;
		}
		//
		bl->~Block();
	}
	//
	Block* MemBank_FirstFit::GetBlockFromPointer( unsigned id, Pointer ptr )
	{
		Address left, right;
		//
		Page& page		= m_bank[id];
		Block* bl		= static_cast<Block*>( page.FirstAlloc );
		Block* nxt		= bl;
		Address addr	= (Address) ptr;
		//
		do
		{
			bl		= nxt;
			left	= ( Address ) bl;
			right	= left + bl->SizeTotal();
			nxt		= bl->Next;
		}
		while( ((addr < left) || (addr > right)) && nxt != PAGE_END_BLOCK );
		//
		if( bl != PAGE_END_BLOCK )
		{
			return bl;
		}
		return 0;
	}
	//
	void MemBank_FirstFit::UpdateMaxChunk()
	{
		size_t max = 0;
		for( unsigned i = 0; i < m_used; ++i )
		{
			Page& page	= m_bank[i];
			if( max < page.MaxChunkFree )
			{
				max = page.MaxChunkFree;
			}
		}
		m_max_available_chunk_size = max;
	}
	//
	void MemBank_FirstFit::UpdateMaxChunkInPage( unsigned id )
	{
		Page& page = m_bank[id];
		if( page.Allocations > 0 )
		{
			size_t diff, max = 0;
			Address left, right;
			Block* bl = static_cast<Block*>( page.FirstAlloc );
			Block* prv = 0, *nxt = 0;
			//
			left	= (Address) page.Start;
			right	= (Address) bl;
			diff	= right - left;
			//
			do
			{
				nxt		= bl->Next;
				if( max < diff )
				{
					max = diff;
				}
				//
				prv		= bl;
				bl		= nxt;
				//
				left	= ( (Address) prv ) + prv->SizeTotal();
				right	= (Address) nxt;
				diff	= right - left;
			}
			while( nxt != PAGE_END_BLOCK );
			//
			left	= ( (Address) prv ) + prv->SizeTotal();
			right	= ( (Address) page.Start ) + page.Size;
			diff	= right - left;
			if( max < diff )
			{
				max = diff;
			}
			//
			page.MaxChunkFree = max;
		}
		else
		{
			page.MaxChunkFree = page.Size;
		}
	}
	//
	Block* MemBank_FirstFit::InsertNewAlloc( Pointer ptr, unsigned id, size_t size, Block* prev, Block* next )
	{
		Page& page = m_bank[id];
		//
		Block* bl = new(ptr) Block();
		if( prev != PAGE_END_BLOCK )
		{
			prev->Next = bl;
		}
		if( next != PAGE_END_BLOCK )
		{
			next->Prev = bl;
		}
		bl->Prev = prev;
		bl->Next = next;
		//
		++page.Allocations;
		page.TotalFree -= size;
		if( page.FirstAlloc == 0 )
		{
			page.FirstAlloc = bl;
		}
		else
		{
			Address addr1	= (Address) page.FirstAlloc;
			Address addr2	= (Address) bl;
			if( addr2 < addr1 )
			{
				page.FirstAlloc = bl;
			}
		}
		//
		return bl;
	}
	//
	bool MemBank_FirstFit::FindInPage( unsigned id, size_t size, Pointer& ptr, Block*& prev, Block*& next )
	{
		Page& page = m_bank[id];
		//
		if( page.Allocations < 1 )
		{
			if( page.MaxChunkFree >= size )
			{
				next = prev = PAGE_END_BLOCK;
				ptr = page.Start;
				return true;
			}
			return false;
		}
		else if( page.MaxChunkFree < size )
		{
			return false;
		}
		else
		{
			Block* prv = 0, *nxt = 0;
			Address left, right;
			Block* bl = static_cast<Block*>( page.FirstAlloc );
			size_t diff;
			//
			left	= (Address) page.Start;
			right	= (Address) bl;
			diff	= right - left;
			nxt		= bl->Next;
			while( diff < size && nxt != PAGE_END_BLOCK )
			{
				prv		= bl;
				bl		= nxt;
				left	= ( (Address) prv ) + prv->SizeTotal();
				right	= (Address) nxt;
				diff	= right - left;
				nxt		= bl->Next;
			}
			//
			if( nxt == PAGE_END_BLOCK  && diff < size )
			{
				left	= ( (Address) bl ) + bl->SizeTotal();
				right	= ( (Address) page.Start ) + page.Size;
				diff	= right - left;
				if( diff >= size )
				{
					ptr		= (Pointer) left;
					prev	= bl;
					next	= PAGE_END_BLOCK;
					return true;
				}
				return false;
			}
			else
			{
				ptr		= (Pointer) left;
				prev	= bl->Prev;
				next	= bl;
				return true;
			}
		}
		return false;
	}
	//
	int MemBank_FirstFit::AllocateNewPage( unsigned id, size_t size )
	{
		Page& page = m_bank[id];
		page.Acquire( size );
		return 0;
	}
	//
	int MemBank_FirstFit::PrintPage( int id, std::ostream& os, bool showData )
	{
		os<<id<<": ";
		m_bank[id].Print( os, showData );
		//
		return 0;
	}
	//
	int MemBank_FirstFit::Sweep()
	{
		for( int i = 0; i < m_used; ++i )
		{
			m_bank[i].Release();
		}
		//
		m_used = 0;
		m_max_available_chunk_size = 0;
		//
		return 0;
	}
	//
	size_t MemBank_FirstFit::SizeTotal() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_used; ++i )
		{
			sz += m_bank[i].Size;
		}
		//
		return sz;
	}
	//
	size_t MemBank_FirstFit::SizeFree() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_used; ++i )
		{
			sz += m_bank[i].TotalFree;
		}
		//
		return sz;
	}
	//
	size_t MemBank_FirstFit::SizeUsed() const
	{
		size_t sz_total = 0, sz_free = 0;
		for( unsigned i = 0; i < m_used; ++i )
		{
			sz_total	+= m_bank[i].Size;
			sz_free		+= m_bank[i].TotalFree;
		}
		//
		return sz_total - sz_free;
	}
}