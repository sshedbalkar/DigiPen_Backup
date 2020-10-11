#pragma once
//
#include <cassert>
//
namespace utility
{
	template< typename T, unsigned int Size >
	class Array
	{
	public:
		T& operator[]( unsigned int index )
		{
			assert( index < Size );
			return mElements[ index ];
		}
		//
		const T& operator[]( unsigned int index ) const
		{
			assert( index < Size );
			return mElements[ index ];
		}
		//
		unsigned int Capacity() const
		{
			return Size;
		}
		//
	private:
		T mElements[ Size ];
	};
}