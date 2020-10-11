#pragma once
//
#include <exception>
//
#include "MemDefines.h"
//
namespace Memory
{
	class Wrong_Size: public std::exception
	{
	public:
		Wrong_Size( unsigned sz );
		const char* what() const;
		//
	private:
		unsigned	size;
	};
	//
	class MemBank
	{
	public:
		virtual ~MemBank(){}
		virtual Pointer		Allocate( size_t size )			= 0;
		virtual int			Deallocate( Pointer ptr )		= 0;
		virtual int			Sweep()							= 0;
		virtual size_t		SizeTotal() const				= 0;
		virtual size_t		SizeFree() const				= 0;
		virtual size_t		SizeUsed() const				= 0;
		virtual int			Reset()							= 0;
		virtual void		SetOStream( std::ostream& os )	= 0;

	};
}