#pragma once
//
namespace utility
{
}
//
namespace Memory
{
	class MemAllocatorGen;
	//
	void PrintLeaks();
	void PrintStats();
}
//
namespace Framework
{
	extern Memory::MemAllocatorGen*		g_memAllocator;
	//
	int									GlobalInit();
	int									GlobalFree();
	
}