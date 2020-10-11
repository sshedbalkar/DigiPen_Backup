#pragma once
//
#include <ostream>
//
#include "MemBank.h"
#include "MemDefines.h"
#include "Pool_Static.h"
//
namespace Memory
{
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
		int				Reset();
		void			SetOStream( std::ostream& os );
		//
	public:
		int				Init();
		//
	private:
		Pointer			AllocInvalidSize( size_t size );
		int				DeallocInvalidSize( Pointer ptr );
		//
	private:
		Pool_Static		m_pools[MAX_POOLS];
		UnitAllocSize	m_minSize;
		UnitAllocSize	m_maxSize;
		unsigned		m_count;
		std::ostream*	m_stream;
	};
}