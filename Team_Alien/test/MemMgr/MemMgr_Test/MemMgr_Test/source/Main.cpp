#include <iostream>
//
#include "TestClass1.h"
#include "MemDefines.h"
#include <vector>
//#include "MemBank_FirstFit.h"
#include <exception>
#include <time.h>
#include <stdlib.h>
#include <new>
#include "MemUtility.h"
//#include "Pool.h"
#include "MemBank_Pooled.h"
//
namespace Memory
{
	void PrintLeaks();
	void PrintStats();
}
//
Memory::Address Func5() { return Memory::GetCallingFunction(2); }
Memory::Address Func4() { return Func5(); }
Memory::Address Func3() { return Func4(); }
Memory::Address Func2() { return Func3(); }
//
int main()
{
	////////////////////////////////////////////////////////////////////////////////////////
	/*
	Memory::UnitAllocSize USize = sizeof(TestClass1);
	std::cout<<"Size(TestClass1): "<<USize<<"\n";
	//
	TestClass1 arr[5], *tptr;
	for( int i = 0; i < 5; ++i )
	{
		arr[i].var1 = i * 11;
	}
	for( int i = 0; i < 5; ++i )
	{
		std::cout<<"i: "<<i<<", Var1: "<<arr[i].var1<<"\n";
	}
	std::cout<<"\n";
	Memory::Pointer ptr = arr;
	Memory::Pointer ptr2;
	Memory::Address addr1;
	//
	std::cout<<"Arr[0]: "<<arr<<", Ptr: "<<ptr<<"\n";
	//
	addr1 = (Memory::Address)ptr;
	std::cout<<std::hex<<"Address: "<<addr1<<"\n";
	//
	addr1 += USize;
	std::cout<<std::hex<<"Address: "<<addr1<<"\n";
	//
	ptr2 = (Memory::Pointer)addr1;
	std::cout<<std::hex<<"Ptr2: "<<ptr2<<"\n";
	//
	addr1 += USize;
	ptr2 = (Memory::Pointer)addr1;
	tptr = (TestClass1*)ptr2;
	std::cout<<std::dec<<"Var1: "<<tptr->var1<<"\n";
	*/
	//
	/*
	int* ti1 = new int();
	delete ti1;
	TestClass1* tc1 = new TestClass1();
	tc1->var1 = 123;
	std::cout<<tc1->var1<<"\n";
	{
		TestClass1* tc2 = new TestClass1[1];
	}
	*/
	///////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	std::cout<<"Hello\n";
	{
		std::vector<int> ints;
		std::cout<<"hi 1\n";
		std::vector<int> ints2;
		std::cout<<"hi 2\n";
		//ints.reserve(1000);
		for( unsigned i = 0; i<1000; ++i )
		{
			ints.push_back(i);
		}
		TestClass1* a = new TestClass1();
		delete a;
	}
	//
	Memory::PrintLeaks();
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////
	/*
	size_t sz = 20;
	Memory::RawMemory mem;
	try
	{
		mem.Acquire( sz );
	}
	catch( std::exception& ex )
	{
		std::cout<<ex.what()<<"\n";
	}
	std::cout<<"Size: "<<mem.Size()<<", ptr: "<<mem.GetMemory()<<"\n";
	//unsigned char* data = static_cast<unsigned char*>(mem.GetMemory());
	//for( int i = 0; i < sz; ++i )
	//{
		//std::cout<<std::dec<<i<<":Val: "<<(int)*data<<": "<<std::hex<<(int)*data<<"\n";
		//data += 1;
	//}
	std::cout<<mem;
	*/
	////////////////////////////////////////////////////////////////////////////////////////////
	/*
	size_t sz = 1234567890;
	std::string str = Memory::MemSizeToText( sz );
	std::cout<<sz<<": "<< str.c_str()<<"\n";
	*/
	///////////////////////////////////////////////////////////////////////////////////////////
	/*
	Memory::MemBank bank;
	//bank.PrintPage( 0, std::cout, true );
	Memory::Pointer ptr1 = bank.Allocate( sizeof(int) );
	int* i = new(ptr1) int();
	*i = 0x53;
	bank.PrintPage( 0, std::cout, true );
	//i->~int();
	//bank.Deallocate(i);
	//bank.PrintPage( 0, std::cout, true );
	//
	ptr1 = bank.Allocate( sizeof(int) );
	int* j = new(ptr1) int();
	*j = 0x54;
	bank.PrintPage( 0, std::cout, true );
	//
	ptr1 = bank.Allocate( sizeof(int) );
	int* k = new(ptr1) int();
	*k = 0x55;
	bank.PrintPage( 0, std::cout, true );
	//
	bank.Deallocate(j);
	bank.PrintPage( 0, std::cout, true );
	//
	ptr1 = bank.Allocate( sizeof(int) );
	j = new(ptr1) int();
	*j = 0x56;
	bank.PrintPage( 0, std::cout, true );
	//
	bank.Deallocate(i);
	bank.PrintPage( 0, std::cout, true );
	//
	ptr1 = bank.Allocate( sizeof(int) );
	i = new(ptr1) int();
	*i = 0x57;
	bank.PrintPage( 0, std::cout, true );
	//
	bank.Deallocate(k);
	bank.PrintPage( 0, std::cout, true );
	//
	ptr1 = bank.Allocate( sizeof(int) );
	k = new(ptr1) int();
	*k = 0x53414E4F;
	bank.PrintPage( 0, std::cout, true );
	*/
	//////////////////////////////////////////////////////////////////////////////////////
	/*
	int *i = new int;
	*i = 123;
	delete i;
	*/
	/////////////////////////////////////////////////////////////////////////////////////
	/*
	const unsigned size = 100;
	const unsigned count = 100;
	::srand( time(NULL) );
	TestClass1 *arr;
	clock_t t1, t2, t3, t4;
	double time1 = 0.0, time2 = 0.0;
	unsigned len;
	std::vector< TestClass1* > vec( size, 0 );
	//
	t4 = clock();
	//
	for( unsigned j = 0; j < count; ++j )
	{
		t1 = clock();
		for( unsigned i = 0; i < size; ++i )
		{
			//len = (::rand()%15)+1;
			arr = new TestClass1;
			vec[i] = arr;
		}
		t2 = clock();
		time1 = ( (double)t2 - (double)t1 )/CLOCKS_PER_SEC;
		//std::cout<<"Alloc time: "<<time1<<"\n";
		//
		for( unsigned i = 0; i < size; ++i )
		{
			delete vec[i];
			vec[i] = 0;
		}
		//
		t3 = clock();
		time2 = ( (double)t3 - (double)t2 )/CLOCKS_PER_SEC;
		//
		//std::cout<<"Dealloc time: "<<time2<<"\n";
		//std::cout<<"Total time: "<<time1+time2<<"\n";
	}
	time2 = ( (double)t3 - (double)t4 )/CLOCKS_PER_SEC;
	//
	std::cout<<"GRAND Total time: "<<time2<<"\n";
	*/
	//////////////////////////////////////////////////////////////////
	//unsigned v = 128;
	//std::cout<<v<<": "<<Memory::UpperPowerOfTwo(v)<<"\n";
	//////////////////////////////////////////////////////////////
	/*
	Memory::Pool pool;
	pool.Init( sizeof(TestClass1), 200 );
	Memory::Pointer p1 = pool.Allocate();
	Memory::Pointer p2 = pool.Allocate();
	p2 = pool.Allocate();
	p2 = pool.Allocate();
	TestClass1 *c1 = new(p1) TestClass1();
	pool.Deallocate( p1 );
	pool.Deallocate( p2 );
	p2 = pool.Allocate();
	//pool.Deallocate( p );
	*/
	///////////////////////////////////////////////////////////////////
/*
	size_t sz = sizeof(TestClass1);
	Memory::MemBank_Pooled pool;
	pool.Init();
	Memory::Pointer p1 = pool.Allocate( sz);
	pool.Deallocate( p1 );
	*/
	/////////////////////////////////////////////////////////////////////
	//Memory::InitDebug();
	//Memory::Address a1 = Func2();
	//Memory::PrintAddress( a1 );
	///////////////////////////////////////////////////////////////////
	//int* i = new int;
	TestClass1* t = new TestClass1();
	//
	Memory::PrintLeaks();
	Memory::PrintStats();
	//
	//std::cin.get();
}