#pragma once //Makes sure this header is only included once
#include "Physics.h"
#include "Variant.h"
#include "Transform.h"
#include "Allocator.h"
//
namespace Framework
{
	extern Memory::Allocator* s_allocator;
	enum HistoryDataType {RIGID_TYPE, COMPLEX_TYPE, STATIC_TYPE,ACTIVATION_TYPE,DOMINO_TYPE,NORIGID_TYPE};//add new types here
	class RigidBodyComplexData
	{
	public:
		Vector3 position;
		Quaternion orientation;
	    Vector3 velocity;
		Vector3 rotation;
		std::bitset<16> alivebitset;
		//Vector3 forceAccum;
		//Vector3 torqueAccum;
		//Vector3 acceleration;
		bool dying;
		float transparrency;
		bool isawake;
		bool previousSleeping;
		//
		void* operator new( size_t size )
		{
			//std::cout<<"RigidBodyComplexData: new: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			//std::cout<<"RigidBodyComplexData: delete: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			//std::cout<<"RigidBodyComplexData: new[]: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			//std::cout<<"RigidBodyComplexData: delete[]: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
	};


	class RigidBodyData
	{
	public:
		Vector3 position;
		Quaternion orientation;
	    Vector3 velocity;
		Vector3 rotation;
		//Vector3 forceAccum;
		//Vector3 torqueAccum;
		//Vector3 acceleration;
		bool dying;
		float transparrency;
		bool isawake;
		bool previousSleeping;
		//
		void* operator new( size_t size )
		{
			//std::cout<<"RigidBodyData: new: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			//std::cout<<"RigidBodyData: delete: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			//std::cout<<"RigidBodyData: new[]: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			//std::cout<<"RigidBodyData: delete[]: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
	};

	class DominoData
	{
	public:
		Vector3 position;
		Quaternion orientation;
	    Vector3 velocity;
		Vector3 rotation;
		bool sleeping;
		bool finished;
		bool isInitial;
		//
		void* operator new( size_t size )
		{
			//std::cout<<"DominoData: new: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			//std::cout<<"DominoData: delete: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			//std::cout<<"DominoData: new[]: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			//std::cout<<"DominoData: delete[]: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
	};

	class StaticBodyData
	{
	public:
		Vector3 position;
		Quaternion orientation;
		bool activated;
		//
		void* operator new( size_t size )
		{
			//std::cout<<"StaticBodyData: new: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			//std::cout<<"StaticBodyData: delete: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			//std::cout<<"StaticBodyData: new[]: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			//std::cout<<"StaticBodyData: delete[]: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
	};
	class NoRigid
	{
	public:
		Vec3 position;
		D3DXQUATERNION orientation;
		//
		void* operator new( size_t size )
		{
			//std::cout<<"NoRigid: new: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			//std::cout<<"NoRigid: delete: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			//std::cout<<"NoRigid: new[]: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			//std::cout<<"NoRigid: delete[]: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
	};
	class ActivationData
	{
	public:
		bool activated;
		float timer;
		//
		void* operator new( size_t size )
		{
			//std::cout<<"ActivationData: new: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			//std::cout<<"ActivationData: delete: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			//std::cout<<"ActivationData: new[]: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			//std::cout<<"ActivationData: delete[]: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
	};

	class HistoryData
	{
	public:
		HistoryDataType dataType;
		utility::Object data;
		int objectId;
		bool destroyed;

		HistoryData(){destroyed=false;}

		int GetBytesUsed()
		{
			int mem=0;
			mem += sizeof(HistoryData);
			if (dataType==RIGID_TYPE)
			{
				mem += sizeof(RigidBodyData);
			}
			else if (dataType==STATIC_TYPE)
			{
				mem += sizeof(StaticBodyData);
			}
			return mem;
		}

		~HistoryData()
		{
			if(data)
			{
				if (dataType==RIGID_TYPE)
				{
					RigidBodyData* temp = static_cast<RigidBodyData*>(data);
					delete temp;
				}
				else if (dataType==COMPLEX_TYPE)
				{
					RigidBodyComplexData* temp = static_cast<RigidBodyComplexData*>(data);
					delete temp;
				}
				else if (dataType==STATIC_TYPE)
				{
					StaticBodyData* temp = static_cast<StaticBodyData*>(data);
					delete temp;
				}
				else if (dataType==ACTIVATION_TYPE)
				{
					ActivationData* temp = static_cast<ActivationData*>(data);
					delete temp;
				}
				else if (dataType==DOMINO_TYPE)
				{
					DominoData* temp = static_cast<DominoData*>(data);
					delete temp;
				}
				else if (dataType==NORIGID_TYPE)
				{
					NoRigid* temp = static_cast<NoRigid*>(data);
					delete temp;
				}
			}
			else
			{
				std::cout<<"HISTORY DATA DESTRUCTOR PROBLEM"<<std::endl;
				std::cin.get();
			}
			
		}
		//
		void* operator new( size_t size )
		{
			//std::cout<<"HistoryData: new: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete( void* ptr )
		{
			//std::cout<<"HistoryData: delete: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
		void* operator new[]( size_t size )
		{
			//std::cout<<"HistoryData: new[]: "<<size<<"\n";
			return s_allocator->Allocate( size );
		}
		void operator delete[]( void* ptr )
		{
			//std::cout<<"HistoryData: delete[]: "<<ptr<<"\n";
			s_allocator->Deallocate( ptr );
		}
	};
}
