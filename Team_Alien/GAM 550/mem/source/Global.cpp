#include "Global.h"
#include "ReturnCodes.h"
#include "MemAllocatorGen.h"
//
namespace Framework
{
	Memory::MemAllocatorGen*		g_memAllocator;
	//
	int GlobalInit()
	{
		//
		return RET_SUCCESS;
	}
	//
	int GlobalFree()
	{
		//
		return RET_SUCCESS;
	}
}