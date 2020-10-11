#pragma once
//
#include <iostream>
#include "Variant.h"
#include "Singleton.h"
#include <vector>
#include <list>
//
#include "Allocator.h"
//
#define MAX_HISTORY_SIZE	100000
//
namespace Framework
{
	extern Memory::Allocator* s_allocator;
	enum FramePosition
	{
		FRM_BEGIN = 0,
		FRM_MIDDLE,
		FRM_END
	};
	//
	class HistoryDataListWrapper;
	class GameFrameData
	{
	public:
									GameFrameData( unsigned l_id, unsigned a_id, HistoryDataListWrapper* data = 0 );
		virtual						~GameFrameData();
		void* operator new( size_t size )
		{
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			s_allocator->Deallocate( ptr );
		}
		//
	public:
		unsigned					LogicFrameId;
		unsigned					ActualFrameId;
		HistoryDataListWrapper*		Payload;
		int							GetBytesUsed();
	};
	//
	class GameHistory: public Singleton<GameHistory>
	{
	public:
									GameHistory();
									~GameHistory();
		//
	public:
		int							NewFrameData( unsigned frame_id, HistoryDataListWrapper* data = 0 );
		HistoryDataListWrapper*		GetFrameData( unsigned frame_id );
		int							DeleteFrameDataById( unsigned l_id );
		HistoryDataListWrapper*		GetLastFrameData();
		HistoryDataListWrapper*		GetPrevFrameData( unsigned curr_frame_id, FramePosition& pos );
		HistoryDataListWrapper*		GetNextFrameData( unsigned curr_frame_id, FramePosition& pos );
		HistoryDataListWrapper*		GetCurrentFrameData();
		int							GetBytesUsed();
		int							DeleteSucceedingFrameData();
		int							DeleteAllFrameData();
		void						Initialize();
		//
	private:
		typedef std::list<GameFrameData*>		DataList;
		DataList			m_list;
		DataList::iterator	m_iter;
		DataList::iterator	m_iter_begin;
	};
	extern GameHistory* GAMEHISTORY;
}