#include <iostream>
//
#include "MemBank_Pooled.h"
#include "MemUtility.h"
#include "ReturnCodes.h"
//
namespace Memory
{
	float			l_log2;
	unsigned		l_minExp;
	////////////////////////////////////////////////////
	Wrong_Size::Wrong_Size( unsigned sz )
		:size( sz )
	{
		//
	}
	//
	const char* Wrong_Size::what() const
	{
		return "Given Block size is wrong!";
	}
	//////////////////////////////////////////////////////
	MemBank_Pooled::MemBank_Pooled()
		:m_minSize( 0 ),
		m_maxSize( 0 ),
		m_count( 0 )
	{
		//
	}
	//
	MemBank_Pooled::~MemBank_Pooled()
	{
		Sweep();
	}
	//
	int MemBank_Pooled::Init()
	{
		m_pools[0].Init( 1,			128 );		// block size(in bytes) * #blocks
		m_pools[1].Init( 2,			128 );		// block size(in bytes) * #blocks
		m_pools[2].Init( 4,			8192 );		// block size(in bytes) * #blocks
		m_pools[3].Init( 8,			131072 );	// block size(in bytes) * #blocks
		m_pools[4].Init( 16,		4194304 );
		m_pools[5].Init( 32,		262144 );
		m_pools[6].Init( 64,		16384 );
		m_pools[7].Init( 128,		4096 );
		m_pools[8].Init( 256,		2048 );
		m_pools[9].Init( 512,		2048 );
		m_pools[10].Init( 1024,		2048 );
		m_pools[11].Init( 2048,		1024 );
		m_pools[12].Init( 4096,		16 );
		m_pools[13].Init( 8192,		16 );
		m_pools[14].Init( 16384,	32 );
		m_pools[15].Init( 32768,	8 );
		m_pools[16].Init( 65536,	8 );
		m_pools[17].Init( 131072,	1024 );
		m_pools[18].Init( 262144,	8 );
		m_pools[19].Init( 524288,	8 );
		m_pools[20].Init( 1048576,	8 );
		m_pools[21].Init( 2097152,	8 );
		m_pools[22].Init( 4194304,	8 );
		m_pools[23].Init( 8388608,	8 );
		//
		m_count		= 24;			// total #pools used(count of above)
		//
		m_minSize	= 1;			// min block size
		m_maxSize	= 8388608;		// max block size
		//
		l_log2		= (float) ::log( 2.0 );
		l_minExp	= Round( ::log((float)m_minSize)/l_log2 );
		//
		return RET_SUCCESS;
	}
	//
	Pointer MemBank_Pooled::Allocate( size_t size )
	{
		unsigned sz = UpperPowerOfTwo( size );
		if( sz < m_minSize || sz > m_maxSize )
		{
			std::cout<<"Wrong size: "<<size<<"("<<sz<<")\n";
			return AllocInvalidSize( size );
			//throw Wrong_Size( sz );
			//return 0;
		}
		unsigned lg = Round( ::log((float)sz)/l_log2 );
		lg -= l_minExp;
		//
		return m_pools[lg].Allocate();
	}
	//
	int MemBank_Pooled::Deallocate( Pointer ptr )
	{
		if( ptr == 0 ) return RET_SUCCESS;
		//
		unsigned i;
		for( i = 0; i < m_count && m_pools[i].Deallocate( ptr ); ++i )
		{
			//
		}
		//
		if( i == m_count )
		{
			std::cout<<"Freeing invalid size ptr: "<<ptr<<"\n";
			return DeallocInvalidSize( ptr );
		}
		//
		return RET_SUCCESS;
	}
	//
	Pointer MemBank_Pooled::AllocInvalidSize( size_t size )
	{
		return ::malloc( size );
	}
	//
	int MemBank_Pooled::DeallocInvalidSize( Pointer ptr )
	{
		//::free( ptr );
		return RET_SUCCESS;
	}
	//
	int MemBank_Pooled::Sweep()
	{
		return RET_SUCCESS;
	}
	//
	size_t MemBank_Pooled::SizeTotal() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i].SizeTotal();
		}
		//
		return sz;
	}
	//
	size_t MemBank_Pooled::SizeFree() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i].SizeFree();
		}
		//
		return sz;
	}
	//
	size_t MemBank_Pooled::SizeUsed() const
	{
		size_t sz = 0;
		for( unsigned i = 0; i < m_count; ++i )
		{
			sz += m_pools[i].MaxAllocs() * m_pools[i].UnitSize();
		}
		//
		return sz;
	}
	//
	int MemBank_Pooled::Reset()
	{
		for( unsigned i = 0; i < m_count; ++i )
		{
			m_pools[i].Reset();
		}
		//
		return 0;
	}
	//
	void MemBank_Pooled::SetOStream( std::ostream& os )
	{
		m_stream = &os;
	}
	//
	//////////////////////////////////////////////////////////////////////////////
	std::ostream& operator<<( std::ostream& os, const MemBank_Pooled& bank )
	{
		unsigned s1 = bank.SizeTotal();
		unsigned s2 = bank.SizeUsed();
		os<<"Total size: "<<MemSizeToText(s1);
		os<<", Used: "<<MemSizeToText(s2)<<"\n";
		for( unsigned i = 0; i < bank.m_count; ++i )
		{
			os<<bank.m_pools[i];
		}
		//
		return os;
	}
}