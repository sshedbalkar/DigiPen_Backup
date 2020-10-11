#pragma once
//
#define USE_MEM_MANAGER				// comment to use the default new and delete operators
//#define MEM_DEBUG					// comment to use the release version of the mem manager
#define MEM_COLLECT_STATS			// comment to not collect stats about memory usage
//#define MEM_USE_NULL_MEM_MANAGER	// comment to use the actual memory manager or it will use a dummy memory manager(which uses malloc/free)
//#define MEM_USE_TRAKCER_ONLY		// comment to use both tracker and allocator(in case of debug) or no tracker and allocator(in case of release) or it will use only tracker(in case of debug)
//
#define MEM_OUTPUT_FILE_PATH				"Memory_Report.txt"
#define MEM_NULL_ADDR						0x00000000
#define MEM_SIZE_1_KB						1024
#define MEM_SIZE_1_MB						1024*1024
#define MEM_SIZE_1_GB						1024*1024*1024
#define MEM_SIZE_1_MBf						1024.0*1024.0
#define MEM_SIZE_1_GBf						1024.0*1024.0*1024.0
#define MEM_POOL_DYNAMIC_FREE_LIST_SIZE		16384
#define CALLSTACK_SIZE						32
//#define USE_MEM_MANAGER_DBG
//
namespace Memory
{
	typedef unsigned char	Byte;
	typedef __int64			Address;
	typedef const char*		Cstr;
	typedef unsigned		UnitAllocSize;
	typedef unsigned		UnitCount;
	typedef void*			Pointer;
	//
	const unsigned char RAW_MEM					= '*';
	const unsigned MAX_MEM_PAGES_PER_BANK		= 256;
	const unsigned MEM_PER_PAGE					= 128 * 1024 * 1024;	// 128 MB
	const Pointer PAGE_END						= 0;
	const unsigned MAX_POOLS					= 32;
	const unsigned MAX_FILE_NAME_LENGTH			= 256;
	//
	const int		ERR_MEM_COULD_NOT_FIND_ADDR	= 23;
	const unsigned	MEM_BLOCK_DIFF_THRESHOLD	= 0x00000008;
	//
	class MemAllocatorGen;
	//
	enum PoolStatus
	{
		MEM_UNALLOCATED,
		MEM_EMPTY,
		MEM_ACTIVE,
		MEM_FULL,
		MEM_CORRUPT
	};
	//
	enum AllocType
	{
		ALLOC_SINGLE,
		ALLOC_ARRAY
	};
}