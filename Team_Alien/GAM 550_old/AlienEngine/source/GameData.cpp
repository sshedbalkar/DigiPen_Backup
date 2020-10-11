#include "Precompiled.h"
#include "GameData.h"
//#include "Archetype.h"
//#include "Precompiled.h"
#include "Utility.h"
#include "resources.h"
//
namespace Framework
{
GameData* GAMEDATA = NULL;
//
GameData::GameData()
	:
	_archetypes(NULL)
{
	//ErrorIf(GAMEDATA!=NULL,"Gamedata already initialized\n");
	GAMEDATA = this;
}
GameData::~GameData()
{
	safeDelete(_archetypes);
	for ( unsigned int i = 0; i < archfiles.size(); ++i )
		safeDelete( archfiles[i] );
	archfiles.clear();
	for ( unsigned int i = 0; i < streams.size(); ++i )
		safeDelete( streams[i] );
	streams.clear();
	for ( unsigned int i = 0; i < readers.size(); ++i )
		safeDelete( readers[i] );
	readers.clear();
}
void GameData::initialize()
{
	_archetypes = new ArchetypeCollection();
}
void GameData::LoadArchetypeData(const char* file)
{
	std::cout<<"Opening Game Settings data file: "<<file<<"\n";
	XMLReader* reader = new XMLReader();

	if(reader->open(file))
	{
		ISerializer* stream = reader->data().clone();
		//
		//
		_archetypes->createCollectionFrom(*stream);

		ArchFileInfo* fileinfo = new ArchFileInfo;
		fileinfo->reader_number = readers.size();
		fileinfo->stream_number = streams.size();
		archfiles.push_back( fileinfo );

		RESOURCES->AddResource( file, static_cast<void*>(fileinfo), this, &GameData::ReloadArchetypeData, 0 );
		//_archetypes->testFn();
		streams.push_back( stream );
	}
	readers.push_back( reader );
}
//======================================================================================
void GameData::ReloadArchetypeData( void* info, const char* file )
{
	ArchFileInfo& fileinfo = *static_cast<ArchFileInfo*>( info );

	std::cout<<"Opening Game Settings data file: "<<file<<"\n";
	XMLReader* reader = new XMLReader();

	if(reader->open(file))
	{
		ISerializer* stream = reader->data().clone();
		//
		_archetypes->createCollectionFrom(*stream);

		safeDelete( readers[fileinfo.reader_number] );
		readers[fileinfo.reader_number] = reader;
		safeDelete( streams[fileinfo.stream_number] );
		streams[fileinfo.stream_number] = stream;
	}
}


}