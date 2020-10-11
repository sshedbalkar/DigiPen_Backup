#pragma once
#include "AntTweakBar.h"
#include "Precompiled.h"
#include "Component.h"
#include <map>
#include "LevelEditor.h"
#include "TweakBarControl.h"

namespace Framework
{


typedef struct {
    float Object1;
    float Object2;
    float Object3;
	float Object4;

} ActivationPropertiesStruct;



class TweakBarManager
{

	typedef std::map<const char*, TweakBarControl*> TweakBarList;
private:
	int m_archEnumId;
	std::map<int, std::string> m_archetypes;
	TweakBarList m_twbarlist;
	GOC * m_selectedGOC;
	enum MyEnum {Object,CreateObject};//add new types here
	const char* GetStringFromEnum(MyEnum);
	MyEnum GetEnumFromString(const char* s);
	TwType ActivationPropertiesStructType;
public:
	TweakBarManager ();
	~TweakBarManager();
	static void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString);
	void ShowObjectDetails ();
	void ShowCreateObjectDetails ();
	static void TW_CALL BuildFromArchetype(void *clientData);
	static void TW_CALL Rotate90X(void *clientData);
	static void TW_CALL Rotate90Y(void *clientData);
	static void TW_CALL Rotate90Z(void *clientData);
	static void TW_CALL SetActivatedBy(const void *value, void *clientData);
	static void TW_CALL GetActivatedBy(void *value, void *clientData);
	static void TW_CALL SetRadius(const void *value, void *clientData);
	static void TW_CALL GetRadius(void *value, void *clientData);


	void InitializeTweakBar();
	void InitializeBars();
	TweakBarControl* GetTweakBarControl( const char* s );
	void HideAllBars();
	void ShowAllBars();
	void SetSelectedGOC(GOC * g);
	void Update();

};

// A user defined structure

 
// Description of the structure (note that you are not required to describe all members, and that members can be reordered)

// Add elem to bar


}
