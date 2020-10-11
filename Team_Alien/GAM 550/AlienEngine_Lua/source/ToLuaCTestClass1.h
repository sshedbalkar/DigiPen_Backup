#pragma once
//
#include "CTestClass1.h"
#include "LuaLibrary.h"
#include <iostream>
#include "LuaHeaders.h"
//
namespace Framework
{
	class ToLuaCTestClass1
	{
	public:
		static int Open( lua_State* L, int* a )
		{
			luabind::module( L)
			[
				luabind::class_< CTestClass1 >( "CTestClass1" )
					.def( luabind::constructor<>() )
					.def( "TestFn1", (int(CTestClass1::*)(int, float)) &CTestClass1::TestFn1 )
					.def( "TestFn2", (int(CTestClass1::*)(const char*)) &CTestClass1::TestFn2 )
					//.def_readonly( "Var2", &CTestClass1::Var2 )
				/*
				luabind::class_< Class2, luabind::bases<CTestClass1> >( "Class2" )
					.def_readonly( "Var3", &Class2::Var3 )
				*/
			];
			//
			return 0;
		}
	};
}