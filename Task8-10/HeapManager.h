// Автор: Алексей Журавлев
// Описание:

#include <Windows.h>
#include <set>
#include <vector>

#pragma once

class CHeapManager {
public:
    CHeapManager();
    ~CHeapManager();

    void Create(int minSize, int maxSize);
    void* Alloc(int size);
    void Free(void* mem);
    void Destroy();

private:
    std::set<LPVOID> freeSmallMemoryBlocks, allocatedBlocks;
	std::set<std::pair<LPVOID, int>> freeMediumMemoryBlocks, freeLargeMemoryBlocks;
	std::vector<int> pages;
	LPVOID heapHead;
	int maxSize;

	bool commitPages(LPVOID start, int numberOfPages);
	LPVOID commitAndUpdate(LPVOID block, int blockSize, int requiredSize);
	void addFreeBlock(LPVOID start, int size);
	std::pair<int, int> getStartAndEndPages(LPVOID block, int blockSize);
	template <typename setType>	std::pair<LPVOID, int> findEmptyBlockAndErase(
		std::set<setType>& targetSet, int size);
	int getBlockSize(LPVOID pointer);
	int getBlockSize(std::pair<LPVOID, int> pair);
	LPVOID getBlock(LPVOID pointer);
	LPVOID getBlock(std::pair<LPVOID, int> pair);
	int roundTo(int number, int divider);
	void addEmptyBlockWithMerge(LPVOID block, int BlockSize);
	template <typename setType>	std::pair<LPVOID, int> tryLeftMerge(std::set<setType>& targetSet, setType compareElement);
	template <typename setType>	std::pair<LPVOID, int> tryRightMerge(std::set<setType>& targetSet, setType compareElement);
	std::pair<LPVOID, int> getBestMergeResult(std::pair<LPVOID, int> first, std::pair<LPVOID, int> second,
		std::pair<LPVOID, int> third);

	static const int pageSize, mediumMemoryBlockMaxSize;

	static int getPageSize();
};