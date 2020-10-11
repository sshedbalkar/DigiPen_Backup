#pragma once
//
#include "MemDefines.h"
//
namespace Memory
{
	class Block
	{
	public:
		Block*					Next;
		Block*					Prev;
		size_t					SizePayload;
		Pointer					Payload;
		static const size_t		SizeBlock;
		//
	public:
		Block();
		~Block();
		//
	public:
		size_t		SizeTotal() const;
		//
		Pointer		SetPayload( size_t size );
	};
	//
	extern Block* const PAGE_END_BLOCK;
}