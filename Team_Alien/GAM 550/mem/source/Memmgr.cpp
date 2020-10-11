#include <iostream>
//
#include "Memmgr.h"
//
Memory::Memmgr		l_allocator;
//
namespace Memory
{
	Memmgr::Memmgr()
		:Var1( 100 )
	{
		std::cout<<"Ctor: Memmgr\n";
	}
	Memmgr::~Memmgr()
	{
		std::cout<<"Dtor: Memmgr\n";
	}
	void* Memmgr::Allocate( size_t size )
	{
		std::cout<<"Memmgr::Allocate: "<<size<<"\n";
		++Var1;
		void* p = ::malloc( size );
		return p;
	}
	void Memmgr::Deallocate( void* ptr )
	{
		std::cout<<"Memmgr::Deallocate: "<<ptr<<"\n";
		--Var1;
		::free( ptr );
	}
}
/////////////////////////////////////////////////////////////////////////////////

#pragma warning( push )
#pragma warning( disable: 4290 )
#ifdef USE_MEM_MANAGER_DBG
//
void*	operator new( size_t size ) throw( std::bad_alloc ) {return l_allocator.Allocate( size );}
void	operator delete( void* ptr ) throw() {l_allocator.Deallocate( ptr );}
//
void*	operator new( size_t size, const std::nothrow_t& ) throw() {return l_allocator.Allocate( size );}
void	operator delete( void* ptr, const std::nothrow_t& ) throw() {l_allocator.Deallocate( ptr );}
//
void*	operator new[]( size_t size ) throw( std::bad_alloc ) {return l_allocator.Allocate( size );}
void	operator delete[]( void* ptr ) throw() {l_allocator.Deallocate( ptr );}
//
void*	operator new[]( size_t size, const std::nothrow_t& ) throw() {return l_allocator.Allocate( size );}
void	operator delete[]( void* ptr, const std::nothrow_t& ) throw() {l_allocator.Deallocate( ptr );}
//
#endif
#pragma warning(pop)

//////////////////////////////////