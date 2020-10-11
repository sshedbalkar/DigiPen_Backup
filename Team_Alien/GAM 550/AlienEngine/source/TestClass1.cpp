#include <iostream>
//
#include "TestClass1.h"
//
/*
namespace Memory
{
	Allocator* TestClass1::m_allocator = 0;
	TestClass1::TestClass1()
	{
		std::cout<<"TestClass1 ctor\n";
	}
	//
	TestClass1::~TestClass1()
	{
		std::cout<<"TestClass1 dtor\n";
	}
	//
	void* TestClass1::operator new( size_t size )
	{
		std::cout<<"TestClass1 new: "<<size<<"\n";
		//return ::malloc( size );
		return m_allocator->Allocate( size );
	}
	//
	void TestClass1::operator delete( void* ptr )
	{
		std::cout<<"TestClass1 delete: "<<ptr<<"\n";
		//::free( ptr );
		m_allocator->Deallocate( ptr );
	}
	//
	void* TestClass1::operator new[]( size_t size )
	{
		std::cout<<"TestClass1 new[]: "<<size<<"\n";
		//return ::malloc( size );
		return m_allocator->Allocate( size, ALLOC_ARRAY );
	}
	//
	void TestClass1::operator delete[]( void* ptr )
	{
		std::cout<<"TestClass1 delete[]: "<<ptr<<"\n";
		//::free( ptr );
		m_allocator->Deallocate( ptr, ALLOC_ARRAY );
	}
}
*/
namespace Memory
{
	template <unsigned T>
	std::ostream& operator<<( std::ostream& os, const TestClass1<T>& rhs)
	{
		os << "hello: "<<rhs.var1;
		return os;
	}
}