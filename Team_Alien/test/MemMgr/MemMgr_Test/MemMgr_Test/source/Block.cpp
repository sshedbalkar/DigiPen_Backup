#include "Block.h"
//
namespace Memory
{
	Block* const PAGE_END_BLOCK		= static_cast<Block*>(PAGE_END);
	const size_t Block::SizeBlock	= sizeof( Block );
	//
	Block::Block()
		:Next( 0 ),
		Prev( 0 ),
		SizePayload( 0 ),
		Payload( 0 )
	{
		//
	}
	//
	Block::~Block()
	{
		Next			= 0;
		Prev			= 0;
		SizePayload		= 0;
		Payload			= 0;
	}
	//
	size_t Block::SizeTotal() const
	{
		return SizeBlock + SizePayload;
	}
	//
	Pointer Block::SetPayload( size_t size )
	{
		Payload			= this + 1;
		SizePayload		= size;
		//
		return Payload;
	}
}