// Автор: Алексей Журавлев
// Описание: Вспомогательные классы для тестирования менеджера кучи

#include <cmath>

#pragma once

CHeapManager globalHeapManager;
HANDLE defaultHeapHandle;

template <unsigned int memberSize>
class CMyHeapTest {
public:
	void* operator new(unsigned int size) {
		return globalHeapManager.Alloc(size * memberSize);
	}

	void operator delete(void* pointer) {
		return globalHeapManager.Free(pointer);
	}
};

template <unsigned int memberSize>
class CDefaultHeapTest {
public:
	void* operator new(unsigned int size){
		return HeapAlloc(defaultHeapHandle, 0, size * memberSize);
	}

	void operator delete(void* pointer) {
		HeapFree(defaultHeapHandle, 0, pointer);
	}
};

class CMyHeapRandom {
public:
	void* operator new(unsigned int size){
		int memberSize = rand() % (1024 * 1024);
		return globalHeapManager.Alloc(size * memberSize);
	}

	void operator delete(void* pointer) {
		return globalHeapManager.Free(pointer);
	}
};

class CDefaultHeapRandom {
public:
	void* operator new(unsigned int size){
		int memberSize = rand() % (1024 * 1024);
		return HeapAlloc(defaultHeapHandle, 0, size * memberSize);
	}

	void operator delete(void* pointer) {
		HeapFree(defaultHeapHandle, 0, pointer);
	}
};