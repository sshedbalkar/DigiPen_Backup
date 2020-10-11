#pragma once

namespace Framework
{

class GraphicsObject;
class ISerializer;

class Material
{
	typedef ObjectLinkList<GraphicsObject> ObjectListType;
	struct EntryFormat
	{
		std::string title;
		char type;
		int num_values;
		int location;
	};
	typedef std::vector<EntryFormat*> EntryListType;

private:
	ObjectListType object_list;
	EntryListType serial_entries;
	int data_size;

public:
	Material();
	~Material();
	void AddAndSerializeObject( GraphicsObject* go, ISerializer* stream );
	char* GfxObjLocateData( GraphicsObject* go, const char* id );
	void RemoveObject( GraphicsObject* go );
	void SetSerializer( const char* content );
	GraphicsObject* Begin() { return object_list.begin(); };
	GraphicsObject* End() { return object_list.end(); };

private:
	Material( Material& ) {};
	void operator= ( Material&) {};

};

}
