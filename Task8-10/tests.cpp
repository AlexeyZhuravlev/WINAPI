// Автор: Алексей Журавлев
// Описание: Файл с тестами, написанными на платформе Google Test.

#include "gtest/gtest.h"
#include "HeapManager.h"
#include <algorithm>

TEST(DummyTests, MakePairRef) {
	int a, b;
	a = b = 0;
	std::make_pair(std::ref(a), std::ref(b)) = std::make_pair(1, 2);
	ASSERT_EQ(a, 1);
	ASSERT_EQ(b, 2);
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