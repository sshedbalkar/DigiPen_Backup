#include <iostream>
//
#include "TestClass1.h"
#include <vector>
#include <exception>
#include <time.h>
#include <stdlib.h>
//
int main()
{
	/////////////////////////////////////////////////////////////////////////////////////
	/*
	const unsigned size = 10000;
	::srand( time(NULL) );
	TestClass1 *arr;
	clock_t t1, t2;
	double time = 0.0;
	unsigned len;
	std::vector< TestClass1* > vec( size, 0 );
	//
	t1 = clock();
	//
	for( unsigned i = 0; i < size; ++i )
	{
		len = (::rand()%15)+1;
		arr = new TestClass1[len];
		vec[i] = arr;
		if( i > 5 && (::rand()%3) < 1 )
		{
			len = ::rand()%i;
			delete[] vec[len];
			vec[len] = 0;
		}
	}
	//std::cin.get();
	//
	for( unsigned i = 0; i < size; ++i )
	{
		delete[] vec[i];
		vec[i] = 0;
	}
	//
	t2 = clock();
	time = ( (double)t2 - (double)t1 )/CLOCKS_PER_SEC;
	//
	std::cout<<"elapsed time: "<<time<<"\n";
	*/
	//////////////////////////////////////////////////////////////////////
	const unsigned size = 10000;
	size_t sz = sizeof(TestClass1);
	void* ptr = ::malloc( sz );
	clock_t t1, t2, t3;
	double time1 = 0.0, time2 = 0.0;
	TestClass1 *tc;
	std::vector< TestClass1* > vec( size, 0 );
	//
	t1 = clock();
	//
	for( unsigned i = 0; i < size; ++i )
	{
		tc = new TestClass1;
		//tc = (TestClass1*) ::malloc( sz );
		//tc = new(ptr) TestClass1();
	}
	t2 = clock();
	time1 = ( (double)t2 - (double)t1 )/CLOCKS_PER_SEC;
	//
	std::cout<<"Alloc time: "<<time1<<"\n";
	//
	std::cin.get();
}