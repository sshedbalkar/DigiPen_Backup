#pragma once
//
#include "MemDefines.h"
namespace Memory
{
	class Memmgr
	{
	public:
		Memmgr();
		~Memmgr();
		void*	Allocate( size_t size );
		void	Deallocate( void* ptr );
	private:
		unsigned Var1;
	};
}