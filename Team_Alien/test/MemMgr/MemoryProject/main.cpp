
#include <stdio.h>
#include <new>

#define OverloadedNew() \
  static void* operator new(size_t size); \
  static void operator delete(void* pMem, size_t size); \

class MemoryPool
{
public:
	MemoryPool(size_t classSize)
	{
	}
	void* Allocate(size_t size){ return malloc(size);}
	void Deallocate(void* pmem, size_t size){free(pmem);}
};


class MemoryHeap
{
public:
	MemoryHeap(size_t classSize)
	{
	}
	void* Allocate(size_t size){ return malloc(size);}
	void Deallocate(void* pmem, size_t size){free(pmem);}
};

#define PooledNew(memoryType, className)  \
  memoryType mPool##className(sizeof(className)); \
  void* className::operator new(size_t size)  { \
	  return mPool##className.Allocate(size); } \
  void className::operator delete(void* pMem, size_t size) { \
	  mPool##className.Deallocate(pMem, size); } 



class SomeClass
{
public:
	//OverloadedNew();
	virtual ~SomeClass(){}
	int a;
	int x;
};


//PooledNew(MemoryHeap, SomeClass);

class AnotherClass : public SomeClass
{
public:
	int a; 
	int b;
};

 // void* SomeClass::operator new(size_t size)
 // {
	//  printf("SomeClass new %d\n", size);
	//  return malloc(size);
 // }
 // void SomeClass::operator delete(void* pMem, size_t size)
 // {
	//  printf("SomeClass delete %d\n", size);
	//  free(pMem);

 // }
 // void* SomeClass::operator new (size_t size, void* ptr)
 // {
	//  printf("SomeClass placement new  %d\n", size);
	//  return ptr;
 // }

 // void SomeClass::operator delete(void* mem, void* ptr)
 // {
	//printf("SomeClass placment delete\n");

 // }

#include <vector>
#include <iostream>
void PrintMemoryLeaks();

int main()
{
	std::cout<<"Hello\n";
	{
		std::vector<int> ints;
		std::cout<<"hi 1\n";
		std::vector<int> ints2;
		std::cout<<"hi 2\n";
		//ints.reserve(1000);
		for(unsigned i = 0;i<1000;++i)
			ints.push_back(i);

		SomeClass* a = new AnotherClass();
		//delete a;
	}

PrintMemoryLeaks();
std::cin.get();
}


//#define malloc(size) AllocateMemory(__LINE__, __FILE__, size);
