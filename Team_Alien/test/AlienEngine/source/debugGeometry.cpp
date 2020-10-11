#include "debugGeometry.h"
#include "Graphics.h"

namespace Framework
{
//==============================================
void DebugGeometry::Initialize()
{
	GRAPHICS->RegisterDebugGeometry( this );
}
//==============================================
void DebugGeometry::Free()
{
	GRAPHICS->RemoveDebugGeometry( this );
}

}