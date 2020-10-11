#include <cstdlib>
#include <iostream>
//
#include "Allocator.h"
#include "ReturnCodes.h"
#include "MemUtility.h"
//
namespace Memory
{
	Allocator::Allocator()
	{
		//
	}
	//
	Allocator::~Allocator()
	{
		Sweep();
	}
	//
	int Allocator::Sweep()
	{
		return RET_SUCCESS;
	}
	//
	int Allocator::Init( UnitAllocSize size, UnitCount max )
	{
		return RET_SUCCESS;
	}
	//
	int Allocator::Status()
	{
		return MEM_ACTIVE;
	}
	//
	Pointer Allocator::Allocate( size_t size, AllocType type )
	{
		//std::cout<<"Allocator::Allocate: "<<size<<"\n";
		//return ::operator new( size );
		return ::malloc( size );
	}
	//
	int Allocator::Deallocate( Pointer mem, AllocType type )
	{
		//std::cout<<"Allocator::Deallocate: "<<mem<<"\n";
		//::operator delete( mem );
		::free( mem );
		return RET_SUCCESS;
	}
	//
	void Allocator::PrintLeaks( std::ostream& os )
	{
		//
	}
	//
	void Allocator::PrintStats( std::ostream& os )
	{
		//
	}
	//
	int Allocator::Reset()
	{
		return RET_SUCCESS;
	}
	//
	void Allocator::SetOStream( std::ostream& os )
	{

	}
}