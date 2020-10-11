#include <iostream>
//
#include "MemAllocatorGen.h"
//
namespace Memory
{

	MemAllocatorGen		g_memAllocator;
	//
	MemAllocatorGen::MemAllocatorGen()
	{
		m_pool.Init( 1, 1 );
		m_ofile.open( MEM_OUTPUT_FILE_PATH, std::ios::out|std::ios::trunc );
		if( !m_ofile.is_open() )
		{
			std::cout<<"Could not open: "<<MEM_OUTPUT_FILE_PATH<<" for writing!\n";
		}
	}
	//
	MemAllocatorGen::~MemAllocatorGen()
	{
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
	}
	//
	void MemAllocatorGen::PrintStats()
	{
		m_pool.PrintStats( m_ofile );
	}
	///////////////////////////////////////////////////////////////////////////////
	void PrintLeaks()
	{
		g_memAllocator.PrintLeaks();
	}
	//
	void PrintStats()
	{
		g_memAllocator.PrintStats();
	}
}
//
/////////////////////////////////////////////////////////////////////////////////

#pragma warning( push )
#pragma warning( disable: 4290 )
//
void*	operator new( size_t size ) throw( std::bad_alloc ) {return Memory::g_memAllocator.Allocate( size );}
void	operator delete( void* ptr ) throw() {Memory::g_memAllocator.Deallocate( ptr );}
//
void*	operator new( size_t size, const std::nothrow_t& ) throw() {return Memory::g_memAllocator.Allocate( size );}
void	operator delete( void* ptr, const std::nothrow_t& ) throw() {Memory::g_memAllocator.Deallocate( ptr );}
//
void*	operator new[]( size_t size ) throw( std::bad_alloc ) {return Memory::g_memAllocator.Allocate( size, Memory::ALLOC_ARRAY );}
void	operator delete[]( void* ptr ) throw() {Memory::g_memAllocator.Deallocate( ptr,  Memory::ALLOC_ARRAY );}
//
void*	operator new[]( size_t size, const std::nothrow_t& ) throw() {return Memory::g_memAllocator.Allocate( size,  Memory::ALLOC_ARRAY );}
void	operator delete[]( void* ptr, const std::nothrow_t& ) throw() {Memory::g_memAllocator.Deallocate( ptr,  Memory::ALLOC_ARRAY );}
//
#pragma warning(pop)

//////////////////////////////////