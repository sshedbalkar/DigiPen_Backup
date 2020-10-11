#pragma once
//
#include <exception>
#include <ostream>
//
#include "MemDefines.h"
//
namespace Memory
{
	class Page;
	//
	class No_Memory: public std::exception
	{
	public:
		const char* what() const;
	};
	/////////////////////////////////////
	class RawMemory
	{
	public:
		RawMemory();
		~RawMemory();
		//
		friend std::ostream&	operator<<( std::ostream& os, const RawMemory& mem );
		//
	public:
		int				Acquire( size_t size );
		int				Release();
		size_t			Size();
		Pointer			GetMemory();
		//
	private:
		Pointer			m_mem;
		size_t			m_size;
	};
}