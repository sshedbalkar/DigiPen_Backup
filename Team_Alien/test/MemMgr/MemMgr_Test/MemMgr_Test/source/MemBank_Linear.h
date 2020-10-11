#pragma once
//
#include <ostream>
//
#include "MemBank.h"
#include "MemDefines.h"
#include "Page.h"
//
namespace Memory
{
#define OPTIMAL_THRESHOLD		(1 - 0.8)
	//
	class Block;
	//
	class MemBank_Linear: public MemBank
	{
	public:
		MemBank_Linear();
		~MemBank_Linear();
		//
	public:
		Pointer		Allocate( size_t size );
		int			Deallocate( Pointer ptr );
		int			Sweep();
		int			PrintPage( int id, std::ostream& os, bool showData = false );
		size_t		SizeTotal() const;
		size_t		SizeFree() const;
		size_t		SizeUsed() const;
		//
	private:
		int			AllocateNewPage( unsigned id, size_t size );
		bool		FindInPage( unsigned id, size_t size, Pointer& ptr, Block*& prev, Block*& next );
		Block*		InsertNewAlloc( Pointer ptr, unsigned id, size_t size, Block* prev, Block* next );
		Block*		GetBlockFromPointer( unsigned id, Pointer ptr );
		void		UpdateMaxChunkInPage( unsigned id );
		void		DeleteAlloc( unsigned id, Block* block );
		void		UpdateMaxChunk();
		//
	private:
		Page		m_bank[MAX_MEM_PAGES_PER_BANK];
		unsigned	m_used;
		unsigned	m_max_available_chunk_size;
	};
}