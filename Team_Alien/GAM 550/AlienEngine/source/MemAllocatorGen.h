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
	#ifdef MEM_USE_NULL_MEM_MANAGER
#include "Allocator.h"
	#else
		#ifdef MEM_USE_TRAKCER_ONLY
#include "Allocator_TrackerOnly.h"
		#else
#include "AllocatorDebug.h"
		#endif
	#endif
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
		Allocator*		GetCustomAllocator( UnitCount maxUnits );
		//
	private:
#ifdef MEM_DEBUG
	#ifdef MEM_USE_NULL_MEM_MANAGER
		Allocator			m_pool;
	#else
		#ifdef MEM_USE_TRAKCER_ONLY
		Allocator_TrackerOnly m_pool;
		#else
		AllocatorDebug		m_pool;
		#endif
	#endif
#else
		AllocatorRelease	m_pool;
#endif
		std::ofstream		m_ofile;
		AllocatorCustom*	m_custom[MAX_POOLS];
		UnitCount			m_count;
	};
	//
	void	PrintLeaks();
	void	PrintStats();
}