// Автор: Алексей Журавлев
// Описание: Файл с тестами, написанными на платформе Google Test.

#define NOMINMAX
#include "gtest/gtest.h"
#include "HeapManager.h"
#include "TestClasses.h"
#include <Psapi.h>
#include <algorithm>


TEST(ManualTests, TestSmallBuffers) {
	CHeapManager manager;
	manager.Create(1 << 14, 1 << 25);
	void* block = manager.Alloc(1 << 15);
	int* first = static_cast<int*>(manager.Alloc(sizeof(int)));
	int* second = static_cast<int*>(manager.Alloc(sizeof(int)));
	int* third = static_cast<int*>(manager.Alloc(sizeof(int)));
	int* fourth = static_cast<int*>(manager.Alloc(sizeof(int)));
	int* fifth = static_cast<int*>(manager.Alloc(sizeof(int)));
	manager.Free(block);
	manager.Free(second);
	manager.Free(fourth);
	manager.Free(third);
	ASSERT_EQ(fourth - first, 6);
	int* sudden = static_cast<int*>(manager.Alloc(2 * sizeof(int)));
	ASSERT_EQ(sudden - first, 2);
	manager.Free(first);
	manager.Free(fifth);
	manager.Free(sudden);
	manager.Destroy();
}



TEST(ManualTests, ManualTestCreateAllocFreeDestroy)
{
	CHeapManager manager;
	manager.Create(1 << 20, 1 << 25);
	int* value = static_cast<int*>(manager.Alloc(sizeof(int)));
	*value = 1000;
	int* array = static_cast<int*>(manager.Alloc(1000 * sizeof(int)));
	for (int i = 0; i < 1000; i++) {
		array[i] = i;
	}
	for (int i = 0; i < 1000; i++) {
		ASSERT_EQ(array[i], i);
	}
	ASSERT_EQ(*value, 1000);
	manager.Free(value);
	for (int i = 0; i < 1000; i++) {
		ASSERT_EQ(array[i], i);
	}
	manager.Free(array);
	manager.Destroy();
}

TEST(ManualTests, TestLargeBuffers) 
{
	CHeapManager manager;
	manager.Create(1 << 14, 1 << 25);
	int* array1 = static_cast<int*>(manager.Alloc((1 << 20) * sizeof(int)));
	std::fill_n(array1, 1 << 20, 1);
	int* array2 = static_cast<int*>(manager.Alloc((1 << 20) * sizeof(int)));
	ASSERT_EQ(array2 - array1, (1 << 20) + 1);
	std::fill_n(array2, 1 << 20, 2);
	int* array3 = static_cast<int*>(manager.Alloc((1 << 20) * sizeof(int)));
	std::fill_n(array3, 1 << 20, 3);
	int* array4 = static_cast<int*>(manager.Alloc((1 << 20) * sizeof(int)));
	std::fill_n(array4, 1 << 20, 4);
	int* array5 = static_cast<int*>(manager.Alloc((1 << 20) * sizeof(int)));
	std::fill_n(array5, 1 << 20, 5);
	ASSERT_EQ(array5 - array1, 4 * ((1 << 20) + 1));
	for (int i = 0; i < (1 << 20); i++) {
		ASSERT_EQ(array1[i], 1);
		ASSERT_EQ(array2[i], 2);
		ASSERT_EQ(array3[i], 3);
		ASSERT_EQ(array4[i], 4);
		ASSERT_EQ(array5[i], 5);
	}
	manager.Free(array2);
	manager.Free(array4);
	for (int i = 0; i < (1 << 20); i++) {
		ASSERT_EQ(array1[i], 1);
		ASSERT_EQ(array3[i], 3);
		ASSERT_EQ(array5[i], 5);
	}
	int* newArray1 = static_cast<int*>(manager.Alloc((1 << 21) * sizeof(int)));
	std::fill_n(newArray1, 1 << 21, 21);
	ASSERT_EQ(newArray1 - array1, 5 * ((1 << 20) + 1));
	for (int i = 0; i < (1 << 20); i++) {
		ASSERT_EQ(array1[i], 1);
		ASSERT_EQ(array3[i], 3);
		ASSERT_EQ(array5[i], 5);
	}
	manager.Free(array3);
	int* newArray2 = static_cast<int*>(manager.Alloc((1 << 21) * sizeof(int)));
	std::fill_n(newArray2, 1 << 21, 22);
	ASSERT_EQ(newArray2 - array1, ((1 << 20) + 1));
	for (int i = 0; i < (1 << 20); i++) {
		ASSERT_EQ(newArray1[i], 21);
		ASSERT_EQ(newArray2[i], 22);
	}
	manager.Free(array1); 
	manager.Free(array5);
	manager.Free(newArray1);
	manager.Free(newArray2);
	manager.Destroy();
}



TEST(CustomClassesManualTests, TestMyHeapManager) {
	globalHeapManager.Create(1 << 12, 1 << 20);
	CMyHeapTest<1024>* pointer = static_cast<CMyHeapTest<1024>*>(new CMyHeapTest<1024>[100]);
	delete[] pointer;
	globalHeapManager.Destroy();
}

TEST(CustomClassesManualTests, TestDefaultHeapManager) {
	defaultHeapHandle = HeapCreate(0, 1 << 12, 1 << 20);
	CDefaultHeapTest<1024>* pointer = static_cast<CDefaultHeapTest<1024>*>(new CDefaultHeapTest<1024>[100]);
	delete[] pointer;
	HeapDestroy(defaultHeapHandle);
}

TEST(CompareTimeOnBigAmountOfSmallAllocations, DefaultHeapManager) {
	defaultHeapHandle = HeapCreate(0, 1 << 12, 1 << 24);
	int numberOfAllocations = 1000000;
	std::vector<CDefaultHeapTest<4>*> pointer(numberOfAllocations);
	for (int i = 0; i < numberOfAllocations; i++) {
		pointer[i] = static_cast<CDefaultHeapTest<4>*>(new CDefaultHeapTest<4>);
	}
	for (int i = 0; i < numberOfAllocations; i++) {
		delete pointer[i];
	}
	HeapDestroy(defaultHeapHandle);
}

TEST(CompareTimeOnBigAmountOfSmallAllocations, MyHeapManager) {
	globalHeapManager.Create(1 << 12, 1 << 24);
	int numberOfAllocations = 1000000;
	std::vector<CMyHeapTest<4>*> pointer(numberOfAllocations);
	for (int i = 0; i < numberOfAllocations; i++) {
		pointer[i] = static_cast<CMyHeapTest<4>*>(new CMyHeapTest<4>);
	}
	for (int i = 0; i < numberOfAllocations; i++) {
		delete pointer[i];
	}
	globalHeapManager.Destroy();
}

TEST(CompareTimeOnSmallAmountofBigAllocations, DefaultHeapManager) {
	defaultHeapHandle = HeapCreate(0, 1 << 12, 1 << 22);
	for (int i = 0; i < 1024 * 256; i++) {
		CDefaultHeapTest<1024 * 1024>* pointer = static_cast<CDefaultHeapTest<1024 * 1024>*>(new CDefaultHeapTest<1024 * 1024>);
		delete pointer;
	}
	HeapDestroy(defaultHeapHandle);
}

TEST(CompareTimeOnSmallAmountofBigAllocations, MyHeapManager) {
	globalHeapManager.Create(1 << 12, 1 << 22);
	for (int i = 0; i < 1024 * 256; i++) {
		CMyHeapTest<1024 * 1024>* pointer = static_cast<CMyHeapTest<1024 * 1024>*>(new CMyHeapTest<1024 * 1024>);
		delete pointer;
	}
	globalHeapManager.Destroy();
}

TEST(CompareTimeCustomCase, DefaultHeapManager) {
	defaultHeapHandle = HeapCreate(0, 1 << 12, 1 << 30);
	const int N = 1000;
	CDefaultHeapTest<4>* smallBuffers[N];
	CDefaultHeapTest<1024>* middleBuffers[N];
	CDefaultHeapTest<1024 * 1024>* largeBuffers[N];
	for (int i = 0; i < N; i++) {
		smallBuffers[i] = static_cast<CDefaultHeapTest<4>*>(new CDefaultHeapTest<4>);
		largeBuffers[i] = static_cast<CDefaultHeapTest<1024 * 1024>*>(new CDefaultHeapTest<1024 * 1024>);
	}
	for (int i = 0; i < N; i++) {
		delete largeBuffers[i];
	}
	for (int i = 0; i < N; i++) {
		middleBuffers[i] = static_cast<CDefaultHeapTest<1024>*>(new CDefaultHeapTest<1024>);
	}
	for (int i = 0; i < N; i++) {
		delete smallBuffers[i];
	}
	for (int i = 0; i < N; i++) {
		delete middleBuffers[i];
	}
	HeapDestroy(defaultHeapHandle);
}

TEST(CompareTimeCustomCase, MyHeapManager) {
	globalHeapManager.Create(1 << 12, 1 << 30);
	const int N = 1000;
	CMyHeapTest<4>* smallBuffers[N];
	CMyHeapTest<1024>* middleBuffers[N];
	CMyHeapTest<1024 * 1024>* largeBuffers[N];
	for (int i = 0; i < N; i++) {
		smallBuffers[i] = static_cast<CMyHeapTest<4>*>(new CMyHeapTest<4>);
		largeBuffers[i] = static_cast<CMyHeapTest<1024 * 1024>*>(new CMyHeapTest<1024 * 1024>);
	}
	for (int i = 0; i < N; i++) {
		delete largeBuffers[i];
	}
	for (int i = 0; i < N; i++) {
		middleBuffers[i] = static_cast<CMyHeapTest<1024>*>(new CMyHeapTest<1024>);
	}
	for (int i = 0; i < N; i++) {
		delete smallBuffers[i];
	}
	for (int i = 0; i < N; i++) {
		delete middleBuffers[i];
	}
	globalHeapManager.Destroy();
}

TEST(CompareMemoryState, MyHeapManager) {
	globalHeapManager.Create(1 << 12, 1 << 28);
	std::vector<CMyHeapRandom*> pointers;
	int iterations = 1000;
	int allocations = 1000;
	
	for (int j = 0; j < allocations; j++) {
		pointers.push_back(static_cast<CMyHeapRandom*>(new CMyHeapRandom));
	}
	std::random_shuffle(pointers.begin(), pointers.end());
	
	for (int i = 0; i < iterations; i++) {
		for (int j = 0; j < allocations; j++) {
			pointers.push_back(static_cast<CMyHeapRandom*>(new CMyHeapRandom));
		}
		std::random_shuffle(pointers.begin(), pointers.end());
		for (int j = 0; j < allocations; j++) {
			delete *(pointers.end() - 1);
			pointers.pop_back();
		}
	}
	PROCESS_MEMORY_COUNTERS memoryCounters;
	GetProcessMemoryInfo(GetCurrentProcess(), &memoryCounters, sizeof(PROCESS_MEMORY_COUNTERS));
	std::cout << "Memory usage: " << memoryCounters.WorkingSetSize << std::endl;
	for (int i = 0; i < pointers.size(); i++) {
		delete pointers[i];
	}
	globalHeapManager.Destroy();
}


TEST(CompareMemoryState, DefaultHeapManager) {
	defaultHeapHandle = HeapCreate(0, 1 << 12, 1 << 28);
	std::vector<CDefaultHeapRandom*> pointers;
	int iterations = 1000;
	int allocations = 1000;

	for (int j = 0; j < allocations; j++) {
		pointers.push_back(static_cast<CDefaultHeapRandom*>(new CDefaultHeapRandom));
	}
	std::random_shuffle(pointers.begin(), pointers.end());

	for (int i = 0; i < iterations; i++) {
		for (int j = 0; j < allocations; j++) {
			pointers.push_back(static_cast<CDefaultHeapRandom*>(new CDefaultHeapRandom));
		}
		std::random_shuffle(pointers.begin(), pointers.end());
		for (int j = 0; j < allocations; j++) {
			delete *(pointers.end() - 1);
			pointers.pop_back();
		}
	}
	PROCESS_MEMORY_COUNTERS memoryCounters;
	GetProcessMemoryInfo(GetCurrentProcess(), &memoryCounters, sizeof(PROCESS_MEMORY_COUNTERS));
	std::cout << "Memory usage: " << memoryCounters.WorkingSetSize << std::endl;
	for (int i = 0; i < pointers.size(); i++) {
		delete pointers[i];
	}
	globalHeapManager.Destroy();
}