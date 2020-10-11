#include "CTestClass1.h"
#include <iostream>
//
namespace Framework
{
	CTestClass1::CTestClass1()
		:TestVar1( 123 )
	{

	}
	//
	CTestClass1::~CTestClass1()
	{
		//
	}
	//
	int CTestClass1::TestFn1( int a, float b )
	{
		std::cout<<"CTestClass1::TestFn1: "<<a<<": "<<b<<"\n";
		//
		return 0;
	}
	//
	int CTestClass1::TestFn2( const char* str )
	{
		std::cout<<"CTestClass1::TestFn2: "<<str<<"\n";
		//
		return 0;
	}
}