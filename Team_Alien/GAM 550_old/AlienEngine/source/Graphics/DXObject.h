#pragma once

namespace Framework
{
	
template<class T> class DXObjectType;
	
class DXObject
{
	const std::type_info& type;
	unsigned int scope;

public:
	template<class T>
	DXObject( T*, unsigned int _scope ) : type( typeid(T) ), scope( _scope ) {};
	virtual ~DXObject() {};

	bool IsUnderScope( unsigned int against );
	const char* TypeName();
	bool IsType( const std::type_info& other_type );

	template<class T>
	T* Object()
	{
		if ( IsType( typeid(T) ) )
		{
			return static_cast< DXObjectType<T>* >( this )->Object();
		}
		else
		{
			printf( "GetDXObject: type mismatch. will get a NULL pointer.(from %s to %s)\n ", type.name(), typeid(T).name() );
			return NULL;
		}
	}
	template<class T>
	void UpdateObject( T* _object )
	{
		if ( IsType( typeid(T) ) )
			static_cast< DXObjectType<T>* >( this )->UpdateObject( _object );
		else
			printf( "UpdateDXObject: type mismatch. not getting updated(from %s to %s).\n", type.name(), typeid(T).name() );

	}
};

template<class T>
class DXObjectType : public DXObject
{
	T* object;
public:
	DXObjectType( T* _object, unsigned int _scope ) : 
	  DXObject( _object, _scope ), object( _object )
	{
	};
	virtual ~DXObjectType() { if( object != NULL ) object->Release(); object = NULL; };
	T* Object() { return object; }
	void UpdateObject( T* _object) { object->Release(); object = _object; }
	//typeid(T);
};

class DXObjectList
{
	typedef std::list<DXObject*> DXObjectListType;
	DXObjectListType objects;
public:
	DXObjectList();
	~DXObjectList();

	template<class T> DXObject* AddDXObject( T* object, unsigned int scope )
	{
		DXObject* newobj = new DXObjectType<T>( object, scope );
		objects.push_back( newobj );
		return newobj;
	}
	void Sweep( unsigned int scope );
	DXObjectListType::iterator Begin() { return objects.begin(); }
	DXObjectListType::iterator End() { return objects.end(); }
	unsigned int Size() { return objects.size(); }
};

}