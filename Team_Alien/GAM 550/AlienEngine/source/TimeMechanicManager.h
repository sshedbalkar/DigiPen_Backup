#pragma once //Makes sure this header is only included once
#include <list>
#include "Message.h"
#include "HistoryData.h"
#include "Utility.h"
#include "GameHistory.h"
//
namespace Memory
{
	class Allocator;
}
//
namespace Framework
{
	enum TimeState {TIME_NOTSTARTED,TIME_NORMAL,TIME_PAUSED,TIME_BACK,TIME_FORWARD};//not started for engine use only
	extern Memory::Allocator* s_allocator;
	//
	class MessageTimeStateChange : public Message
	{
	public:
		MessageTimeStateChange(TimeState _t) : Message(Mid::TimeStateChange) ,t(_t) {};
		MessageTimeStateChange(const MessageTimeStateChange& rhs)
			:Message(rhs)
		{
			t=rhs.t ;
		}
		TimeState t;

	};


	class HistoryDataListWrapper
	{
		public:
			std::list<HistoryData*> m_historyDataList;
			int GetBytesUsed();
			~HistoryDataListWrapper();
			//
			void* operator new( size_t size );
			void operator delete( void* ptr );
			void* operator new[]( size_t size );
			void operator delete[]( void* ptr );
	};

	class TimeMechanicManager
	{
		
		//friend class RigidBody;
	public:
		TimeMechanicManager();
		~TimeMechanicManager(){}
		void Initialize();
		void Update(float dt);	
	    void SendMessage(Message *);
		void AddTolist(HistoryData * hd);
		void UpdateObjects(HistoryDataListWrapper*,bool isBackwards);
		TimeState GetTimeState(){return m_timeState;}
		void SetTimeState(TimeState t);
		const FramePosition & GetFramePos() const{return framePos;}
		bool IsbTimeListEnd(){ return m_bTimeListEnd; }
		unsigned int GetFrameId(){return m_frameCounter;}
		void SetImmuneObject(GOC *);
		GOC* GetImmuneObject();
		bool Disabled;
		bool BackDisabled;
		bool ForwardDisabled;
		const int  & GetTimeSpeed(){return m_xSpeed;}
	private:
		FramePosition framePos;
		unsigned int m_frameCounter;
		HistoryDataListWrapper *m_container;
		TimeState m_timeState;
		TimeState m_nextFrameState;
		bool m_bTimeListEnd;
		int timecounter;
		bool m_timerStarted;
		GOC * m_immuneObject;
		int m_xSpeed;
		void DecideTimeState();
		void DecideTimeSpeed();

		
	};
	extern TimeMechanicManager* TIMEMANAGER;
}