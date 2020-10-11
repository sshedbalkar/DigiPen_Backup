#include "precompiled.h"
#include <sstream>
#include "material.h"
#include "XMLSerialization.h"
#include "GraphicsObject.h"

namespace Framework
{

#define MAX_STRING_LENGTH	20

//===========================================
Material::Material()
{
	data_size = 0;
}
//===========================================
Material::~Material()
{
	for ( unsigned int i = 0; i < serial_entries.size(); ++i )
	{
		delete( serial_entries[i] );
		serial_entries[i] = NULL;
	}
	serial_entries.clear();
}
//===========================================
void Material::AddAndSerializeObject( GraphicsObject* go, ISerializer* stream )
{
	std::string content;
	go->data = new char [data_size];
	ZeroMemory( go->data, data_size );
	char* ptr = go->data;
	bool valid;
	char ch;
	//*** serialization
	if ( stream )
	{
		for ( unsigned int i = 0; i < serial_entries.size(); ++i )
		{
			valid = stream->readValue( serial_entries[i]->title.c_str(), content );
			if (!valid )
			{
				printf( "#graphics object: no entry found in xml while serializing '%s'.\n", serial_entries[i]->title.c_str() );
				if ( serial_entries[i]->type == 's' ) ptr += MAX_STRING_LENGTH;
				else ptr += sizeof(float) * serial_entries[i]->num_values;
				continue;
			}
			std::stringstream ss( content );

			int count = serial_entries[i]->num_values;
			//serializing floats
			if ( serial_entries[i]->type == 'f' )
			{
				float* floc = (float*)ptr;
				float ftemp;
				while ( count > 0 )
				{
					valid = ss >>ftemp;
					ErrorIf( !valid, "error serializing '%s' of graphics object.", serial_entries[i]->title.c_str() );
					--count;
					if ( count > 0 ) 
					{
						ss >>ch;
						ErrorIf( ch != ',', "error serializing '%s' of graphics object.", serial_entries[i]->title.c_str() );
					}
					*floc = ftemp;
					++floc;
				}
				ptr = (char*)floc;
			}
			//serializing integars
			else if ( serial_entries[i]->type == 'i' )
			{
				int* iloc = (int*)ptr;
				int itemp;
				while ( count > 0 )
				{
					valid = ss >>itemp;
					ErrorIf( !valid, "error serializing '%s' of graphics object.", serial_entries[i]->title.c_str() );
					--count;
					if ( count > 0 ) 
					{
						ss >>ch;
						ErrorIf( ch != ',', "error serializing '%s' of graphics object.", serial_entries[i]->title.c_str() );
					}
					*iloc = itemp;
					++iloc;
				}
				ptr = (char*)iloc;
			}
			else if ( serial_entries[i]->type == 's' )
			{
				char* sloc = (char*)ptr;
				valid = ss >>ptr;
				printf( "%s\n", ptr );
				ptr += MAX_STRING_LENGTH;
			}
		}
	}
	
	object_list.push_back( go );
}
//===========================================
void Material::RemoveObject( GraphicsObject* go )
{
	
	if ( go->data ) { delete [] go->data; go->data = NULL; }
	object_list.erase( go );
}
//===========================================
char* Material::GfxObjLocateData( GraphicsObject* go, const char* id )
{
	EntryListType::iterator it;
	it = serial_entries.begin();
	while ( it != serial_entries.end() )
	{
		if ( (*it)->title == id ) return go->data + (*it)->location;
		++it;
	}
	return NULL;
}
//===========================================
void Material::SetSerializer( const char* content )
{
	std::stringstream ss(content);
	char ch;
	bool valid;
	data_size = 0;

	while ( ss >>ch )
	{
		EntryFormat* entry = new EntryFormat;
		while ( ch != '(' )
		{ entry->title.push_back( ch ); ss >>ch; } 
		valid = (ss >>entry->type);
		ErrorIf( !valid, "material serializer format incorrect.\n" );
		entry->location = data_size;
		if ( entry->type == 's' )
		{
			data_size += MAX_STRING_LENGTH;
		}
		else
		{
			valid = (ss >>entry->num_values);
			ErrorIf( !valid, "material serializer format incorrect.\n" );
			data_size += entry->num_values * sizeof(float);
		}
		ss >>ch;
		ErrorIf( ch != ')', "material serializer format incorrect.\n" );

		serial_entries.push_back( entry );
	}
}

}
