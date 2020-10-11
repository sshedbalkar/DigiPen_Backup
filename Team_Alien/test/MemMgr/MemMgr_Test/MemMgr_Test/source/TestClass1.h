#pragma once
//
#define ARR_SIZE 144
class TestClass1
{
public:
	TestClass1();
	~TestClass1();
	/*
	static void* operator new(size_t size)
	{
		std::cout<<"hello TestClass1\n";
		return ::malloc(size);
	}
	*/
	//
public:
	int var1;
	char str[ARR_SIZE];
};