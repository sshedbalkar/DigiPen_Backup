#pragma once
//
namespace Framework
{
	class CTestClass1
	{
	public:
		CTestClass1();
		~CTestClass1();
		//
		int TestFn1( int a, float b );
		int TestFn2( const char* str );
		//
	private:
		int TestVar1;
		//
	public:
		int Var2;
	};
	//
	/*
	class Class2: public CTestClass1
	{
	public:
		Class2(){}
		~Class2(){}
		//
		int Var3;
	};
	*/
}