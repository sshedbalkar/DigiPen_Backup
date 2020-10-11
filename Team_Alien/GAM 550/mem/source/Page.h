#pragma once
//
#include <ostream>
//
#include "MemDefines.h"
#include "RawMemory.h"
//
namespace Memory
{
	class Page
	{
	public:
		Page();
		~Page();
		//
	public:
		int				Acquire( size_t size );
		int				Release();
		void			Print( std::ostream& os, bool showData );
		//
	private:
		RawMemory		Mem;
		Pointer			Start;
		size_t			TotalFree;
		size_t			MaxChunkFree;
		size_t			Size;
		unsigned		Allocations;
		Pointer			FirstAlloc;
		Pointer			LastAlloc;
	};
}