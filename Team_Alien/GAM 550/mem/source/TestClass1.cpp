#include <iostream>
//
#include "TestClass1.h"
//

namespace Memory
{
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
}