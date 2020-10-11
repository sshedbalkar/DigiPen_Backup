#pragma once
//
#include <fstream>
//
#include "MemDefines.h"
#include "Allocator_Custom.h"
//
#ifdef MEM_DEBUG
#undef MEM_DEBUG
#endif
//
#ifdef MEM_DEBUG
#else
#include "AllocatorRelease.h"
#endif
//
namespace Memory
{
	class MemAllocatorGen
	{
	public:
		MemAllocatorGen();
		~MemAllocatorGen();
		//
	public:
		Pointer			Allocate( size_t size, AllocType type = ALLOC_SINGLE );
		int				Deallocate( Pointer memory, AllocType type = ALLOC_SINGLE );
		void			PrintLeaks();
		void			PrintStats();
		//
	private:
#ifdef MEM_DEBUG
#else
		AllocatorRelease	m_pool;
#endif
		std::ofstream		m_ofile;
		UnitCount			m_count;
	};
	//
	void	PrintLeaks();
	void	PrintStats();
}