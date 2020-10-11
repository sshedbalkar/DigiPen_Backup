#include <iostream>
//
#include "Global.h"
#include "MemAllocatorGen.h"
#include "Pool_Dynamic.h"
#include "ReturnCodes.h"
//
#ifdef USE_MEM_MANAGER
static Memory::MemAllocatorGen		l_allocator;
#endif
//
namespace Memory
{
	bool GAME_EXIT = false;
	MemAllocatorGen::MemAllocatorGen()
		:m_count( 0 )
	{
		std::cout<<"MemAllocatorGen init\n";
		m_pool.Init( 1, 1 );
		m_ofile.open( MEM_OUTPUT_FILE_PATH, std::ios::out|std::ios::trunc );
		if( !m_ofile.is_open() )
		{
			std::cout<<"Could not open: "<<MEM_OUTPUT_FILE_PATH<<" for writing!\n";
		}
		else
		{
			m_pool.SetOStream( m_ofile );
		}
		//
		Framework::g_memAllocator = this;
		//
	}
	//
	MemAllocatorGen::~MemAllocatorGen()
	{
		for(unsigned i = 0; i < m_count; ++i )
		{
			delete m_custom[i];
		}
		m_pool.Sweep();
		m_ofile.close();
	}
	//
	Pointer MemAllocatorGen::Allocate( size_t size, AllocType type )
	{
		return m_pool.Allocate( size, type );
	}
	//
	int MemAllocatorGen::Deallocate( Pointer memory, AllocType type )
	{
		return m_pool.Deallocate( memory, type );
	}
	//
	void MemAllocatorGen::PrintLeaks()
	{
		m_pool.PrintLeaks( m_ofile );
		for(unsigned i = 0; i < m_count; ++i )
		{
			m_custom[i]->PrintLeaks( m_ofile );
		}
	}
	//
	void MemAllocatorGen::PrintStats()
	{
		m_pool.PrintStats( m_ofile );
		for(unsigned i = 0; i < m_count; ++i )
		{
			m_custom[i]->PrintStats( m_ofile );
		}
	}
	//
	Allocator* MemAllocatorGen::GetCustomAllocator( UnitCount maxUnits )
	{
		AllocatorCustom* all = new AllocatorCustom( maxUnits );
		m_custom[m_count] = all;
		++m_count;
		//
		return all;
	}
	///////////////////////////////////////////////////////////////////////////////
	void PrintLeaks()
	{
#ifdef USE_MEM_MANAGER
		Framework::g_memAllocator->PrintLeaks();
#endif
	}
	//
	void PrintStats()
	{
#ifdef USE_MEM_MANAGER
		Framework::g_memAllocator->PrintStats();
#endif
	}
}
//
/////////////////////////////////////////////////////////////////////////////////

#pragma warning( push )
#pragma warning( disable: 4290 )
#ifdef USE_MEM_MANAGER
//
void*	operator new( size_t size ) throw( std::bad_alloc ) {return l_allocator.Allocate( size );}
void	operator delete( void* ptr ) throw() {l_allocator.Deallocate( ptr );}
//
void*	operator new( size_t size, const std::nothrow_t& ) throw() {return l_allocator.Allocate( size );}
void	operator delete( void* ptr, const std::nothrow_t& ) throw() {l_allocator.Deallocate( ptr );}
//
void*	operator new[]( size_t size ) throw( std::bad_alloc ) {return l_allocator.Allocate( size, Memory::ALLOC_ARRAY );}
void	operator delete[]( void* ptr ) throw() {l_allocator.Deallocate( ptr,  Memory::ALLOC_ARRAY );}
//
void*	operator new[]( size_t size, const std::nothrow_t& ) throw() {return l_allocator.Allocate( size,  Memory::ALLOC_ARRAY );}
void	operator delete[]( void* ptr, const std::nothrow_t& ) throw() {l_allocator.Deallocate( ptr,  Memory::ALLOC_ARRAY );}
//
#endif
#pragma warning(pop)

//////////////////////////////////