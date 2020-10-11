#pragma once
//
#include <vector>
#include "FunctionBind.h"
#include "ReturnCodes.h"
//
struct lua_State;
//
namespace Framework
{
	template< typename ClassType, typename Param0Type >
	class Method
	{
	public:
															Method( ClassType* obj, utility::FunctionObject* fn, Param0Type* dt )
																:func( fn ),
																object( obj ),
																data( dt )
															{
																//
															}
															~Method(){};
		//
	public:
		utility::FunctionObject*							func;
		ClassType*											object;
		Param0Type*											data;
	};
	//////////////////////////////////////////////////////////////////////////////
	class LuaLibrary
	{
	public:
															LuaLibrary();
															~LuaLibrary();
		//
		static void											Open( lua_State* L );
		//
		template< typename ClassType, typename Param0Type >
		static inline int Add( int(ClassType::*Func)(lua_State*, Param0Type*), ClassType* obj, Param0Type* data )
		{
			Method< void, void >* m = new Method< void, void >( obj, utility::Bind(Func), data );
			m_function_list.push_back( m );
			//
			return RET_SUCCESS;
		}
		//
		template< typename ClassType >
		static inline int Add( int(ClassType::*Func)(lua_State*), ClassType* obj )
		{
			Method< void, void >* m = new Method< void, void >( obj, utility::Bind(Func), NULL );
			m_function_list.push_back( m );
			//
			return RET_SUCCESS;
		}
		//
		template< typename Param0Type >
		static inline int Add( int(*Func)(lua_State*, Param0Type*), Param0Type* data )
		{
			Method< void, void >* m = new Method< void, void >( NULL, utility::Bind(Func), data );
			m_function_list.push_back( m );
			//
			return RET_SUCCESS;
		}
		//
		static inline int Add( int(*Func)(lua_State*) )
		{
			Method< void, void >* m = new Method< void, void >( NULL, utility::Bind(Func), NULL );
			m_function_list.push_back( m );
			//
			return RET_SUCCESS;
		}
		//
		//void												OpenAllLibs();
		//
	private:
		static void											OpenAllLibs( void );
		//
	private:
		//
		typedef std::vector< Method<void, void>* >			LuaFuncList;
		static LuaFuncList									m_function_list;
	};
}