#pragma once
//
#include "MemDefines.h"
//
namespace Memory
{
	class Pool
	{
	public:
		virtual ~Pool(){}
		virtual int				Init( UnitAllocSize size, UnitCount max )	= 0;
		virtual Pointer			Allocate()									= 0;
		virtual int				Deallocate( Pointer mem )					= 0;
		virtual UnitAllocSize	UnitSize() const							= 0;
		virtual size_t			SizeTotal() const							= 0;
		virtual size_t			SizeFree() const							= 0;
		virtual size_t			SizeUsed() const							= 0;
		virtual unsigned		MaxAllocs() const							= 0;
		virtual unsigned		MaxFrees() const							= 0;
		virtual UnitCount		MaxUnits() const							= 0;
		virtual PoolStatus		Status() const								= 0;
		virtual int				Reset()										= 0;
	};
}