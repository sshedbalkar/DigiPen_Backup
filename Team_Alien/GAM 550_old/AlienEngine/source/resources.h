/// runtime reloadable resources
#include "System.h"

namespace Framework
{

class Resources : public ISystem
{
	///////////////////////////// resource structure ///////////////////////////////////
	struct ResourceReloadable
	{
		std::string path;
		void* handle;
		SYSTEMTIME lastbuild;
		int scope;
		virtual void Refresh() {};
	};
	////////////////// resource structure( with refresh function ) ////////////////////
	template<class T>
	struct ResourceWithFunction : public ResourceReloadable
	{
		T* system;
		void (T::*func)( void*, const char* );
		virtual void Refresh()
		{
			(system->*func)( handle, path.c_str() );
		}
	};
	///////////////////////// list of all reloadable resources /////////////////////////
	typedef std::list<ResourceReloadable*> ResourceListType;
	ResourceListType resource_list;

public:
	Resources();
	~Resources();
	
	virtual void Update( float timeslice );
	virtual void Initialize();
	virtual void Free();
	virtual void SendMessage( Message* msg );
	virtual void Unload();
	virtual std::string GetName() { return "Resources";};

	template<class T>
	void AddResource( const char* path, void* handle, T* system, void (T::*func)( void*, const char* ), int scope = 1 )
	{
		WIN32_FILE_ATTRIBUTE_DATA filedata;
		ResourceWithFunction<T>* res = new ResourceWithFunction<T>;
		res->path = path;
		res->handle = handle;
		res->system = system;
		res->func = func;
		res->scope = scope;
		GetFileAttributesEx( path, GetFileExInfoStandard, &filedata );
		FileTimeToSystemTime( &filedata.ftLastWriteTime, &res->lastbuild );
		resource_list.push_back( res );
	}

private:
	void RefreshResources();

};

extern Resources* RESOURCES;

}