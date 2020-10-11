#pragma once
//
#include <exception>
#include <ostream>
//
#include "MemBank.h"
#include "MemDefines.h"
#include "Pool.h"
//
namespace Memory
{
	class Wrong_Size: public std::exception
	{
	public:
		const char* what() const;
	};
	//
	class MemBank_Pooled: public MemBank
	{
	public:
		MemBank_Pooled();
		~MemBank_Pooled();
		friend std::ostream& operator<<( std::ostream& os, const MemBank_Pooled& bank );
		//
	public:
		Pointer			Allocate( size_t size );
		int				Deallocate( Pointer ptr );
		int				Sweep();
		size_t			SizeTotal() const;
		size_t			SizeFree() const;
		size_t			SizeUsed() const;
		//
	public:
		int				Init();
		//
	private:
		Pool			m_pools[MAX_POOLS];
		UnitAllocSize	m_minSize;
		UnitAllocSize	m_maxSize;
		unsigned		m_count;
	};
}