#pragma once
//
#include "Global.h"
#include "MemAllocatorGen.h"
#include "Allocator.h"
#include <ostream>
//
namespace Memory
{
	template<unsigned U>
	class TestClass1
	{
	public:
		TestClass1(){var1 = U;};
		~TestClass1(){};
		/*
		void* operator new( size_t size );
		void operator delete( void* ptr );
		void* operator new[]( size_t size );
		void operator delete[]( void* ptr );
		static void AcquireAllocator()
		{
			if( !m_allocator )
			{
				m_allocator = Framework::g_memAllocator->GetCustomAllocator( 1024 );
			}
		}
		*/
		template <unsigned U>
		friend std::ostream& operator<<( std::ostream&, const TestClass1<U>& );
		//
		unsigned var1;
		static Allocator* m_allocator;
	};
}
#include "TestClass1.cpp"