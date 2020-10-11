#include <new>
#include <map>
#include <vector>


#include "mallocator.hpp"

#include <Windows.h>
#include <DbgHelp.h>
#pragma comment (lib, "dbghelp.lib")

typedef unsigned char byte;
typedef __int64 Address;
typedef const char * cstr;

enum AllocationType
{
	Single,
	Array,
};

struct AllocationEntry
{
	AllocationEntry()
	{
		size = 0;
		count = 0;
	}

	AllocationType Type;
	Address address;
	size_t size;
	size_t count;
};

struct AddressInfo
{
	cstr File;
	int Line;
	bool Module;
};

typedef std::map<Address, AddressInfo, std::less<Address>, Mallocator< std::pair<Address, AddressInfo> > > MapInfoType;
typedef std::map<void*, AllocationEntry, std::less<void*>, Mallocator< std::pair<void*, AllocationEntry> > > MapType;
typedef std::map<Address, AllocationEntry, std::less<Address>, Mallocator< std::pair<Address, AllocationEntry> > > TrackMap;

void debugprintf(const char *str,...)
{
	va_list mkr;
	char buff[2048];

	va_start(mkr,str);
	vsprintf_s(buff,_countof(buff),str,mkr);
	va_end(mkr);

	OutputDebugStringA(buff);
}

//typedef std::vector< AllocationEntry, Mallocator< AllocationEntry > VectorType;

struct DebugHeap
{
	DebugHeap()
	{
		Count = 0;
		SymInitialize(GetCurrentProcess(),NULL,true);
		SymSetOptions(SymGetOptions() | SYMOPT_LOAD_LINES);
	}

	MapInfoType StackInfo;
	MapType ActiveAllocations;
	TrackMap Allocations;
	int Count;
};

DebugHeap* debugHeap = NULL;

void PrintAddress(Address address)
{
	char* file = "Unknown";
	unsigned line = 0;

	IMAGEHLP_LINE64 hlp = {0};
	DWORD symbol_offset = 0;
	if(SymGetLineFromAddr64(GetCurrentProcess(),address,&symbol_offset,&hlp))
	{
		file = hlp.FileName;
		line = hlp.LineNumber;	
	}
	debugprintf("%s(%u): ", file, line);
}

Address GetCallingFunction(unsigned skipDepth)
{
	CONTEXT c;
#ifdef _WIN64
	RtlCaptureContext(&c);
#else
	c.ContextFlags = CONTEXT_CONTROL;
	__asm
	{
LABEL: mov eax, [LABEL];
		mov c.Eip, eax;
		mov c.Ebp, ebp;
		mov c.Esp, esp;
	}
#endif
	STACKFRAME64 stack_frame = {0};
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrPC.Offset = c.Eip;
	stack_frame.AddrStack.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = c.Esp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = c.Ebp;

	for(unsigned i=0;i<skipDepth;++i)
		StackWalk64(IMAGE_FILE_MACHINE_I386,GetCurrentProcess(),GetCurrentThread(),&stack_frame,&c,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL);

	return stack_frame.AddrPC.Offset;

}

Address GetNearestModule(unsigned skipDepth)
{
	CONTEXT c;
#ifdef _WIN64
	RtlCaptureContext(&c);
#else
	c.ContextFlags = CONTEXT_CONTROL;
	__asm
	{
LABEL: mov eax, [LABEL];
		mov c.Eip, eax;
		mov c.Ebp, ebp;
		mov c.Esp, esp;
	}
#endif
	STACKFRAME64 stack_frame = {0};
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrPC.Offset = c.Eip;
	stack_frame.AddrStack.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = c.Esp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = c.Ebp;

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	for(unsigned i=0;i<skipDepth;++i)
		StackWalk64(IMAGE_FILE_MACHINE_I386,process,thread,&stack_frame,&c,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL);
	
	for(;;)
	{
		Address address = stack_frame.AddrPC.Offset;

		MapInfoType::iterator it = debugHeap->StackInfo.find(address);

		if(it!=debugHeap->StackInfo.end())
		{
			//Adress already set
			if(it->second.Module)
				return address;
		}
		else
		{
			IMAGEHLP_LINE64 hlp = {0};
			DWORD symbol_offset = 0;
			if(SymGetLineFromAddr64(process,address,&symbol_offset,&hlp))
			{
				int l = strlen(hlp.FileName);
				const char* extensionName = hlp.FileName + l - 3;

				AddressInfo info;
				info.Module = strcmp(extensionName, "cpp") == 0;
				info.File = hlp.FileName;
				info.Line = hlp.LineNumber;
				debugHeap->StackInfo[address] = info;

				if(info.Module)
					return address;
			}
		}

		StackWalk64(IMAGE_FILE_MACHINE_I386,process,thread,&stack_frame,&c,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL);

	}

	return stack_frame.AddrPC.Offset;

}

void* Allocate( size_t size, AllocationType type=Single )
{
	if(debugHeap==NULL)
	{
		void* mem = malloc(sizeof(DebugHeap) );
		debugHeap = new(mem) DebugHeap;
	}
	MapType& map = debugHeap->ActiveAllocations;
	void* memory = malloc(size);

	AllocationEntry e;
	e.address = GetNearestModule(4);
	e.size = size;
	e.Type = type;
	e.count = debugHeap->Count;
	++debugHeap->Count;

	map[memory] = e;

	AllocationEntry& allEntry = debugHeap->Allocations[e.address];
	allEntry.address = e.address;
	allEntry.size += size;
	allEntry.count += 1;
	
	return memory;
}

void Deallocate(void* memory, AllocationType type=Single)
{
	MapType& map = debugHeap->ActiveAllocations;

	map.erase(memory);

	free(memory);
}

void PrintMemoryLeaks()
{
	MapType& map = debugHeap->ActiveAllocations;
	MapType::iterator it = map.begin();
	while(it!=map.end())
	{
		AllocationEntry& entry = it->second;

		const char* typeCstr = entry.Type == Single ? "Single" : "Array";
		PrintAddress(entry.address);
		debugprintf(" Memory leak %d\n", entry.size);

		++it;
	}


	{
	TrackMap& trackMap = debugHeap->Allocations;
	TrackMap::iterator it = trackMap.begin();
	while(it!=trackMap.end())
	{
		AllocationEntry& entry = it->second;
		PrintAddress(entry.address);
		debugprintf(" Memory Usage Size: %d Count: %d Avg: %d\n", entry.size, entry.count, entry.size/entry.count);
		++it;
	}
	}
}



#pragma warning(push)
#pragma warning(disable: 4290)
void * operator new(size_t size) throw (std::bad_alloc) {return Allocate(size);}
void * operator new(size_t size, const std::nothrow_t&) throw () {return Allocate(size);}
void * operator new[](size_t size) throw (std::bad_alloc) {return Allocate(size,Array);}
void * operator new[](size_t size, const std::nothrow_t&) throw () {return Allocate(size,Array);}
void operator delete(void * ptr) throw () {Deallocate(ptr);}
void operator delete(void * ptr, const std::nothrow_t&) throw () {Deallocate(ptr);}
void operator delete[](void * ptr) throw () {Deallocate(ptr,Array);}
void operator delete[](void * ptr, const std::nothrow_t&) throw () {Deallocate(ptr,Array);}
#pragma warning(pop)

