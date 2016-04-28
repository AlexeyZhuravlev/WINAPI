// Автор: Алексей Журавлев
// Описание: Реализация методов класса CHeapManager

#define NOMINMAX
#include "HeapManager.h"

const int CHeapManager::pageSize = CHeapManager::getPageSize();
const int CHeapManager::mediumMemoryBlockMaxSize = CHeapManager::pageSize * 32;

CHeapManager::CHeapManager() 
{
	heapHead = 0;
	pages = std::vector<int>();
	freeSmallMemoryBlocks = std::set<LPVOID>();
	freeLargeMemoryBlocks = freeMediumMemoryBlocks = std::set<std::pair<LPVOID, int>>();
}

CHeapManager::~CHeapManager() 
{
}

void CHeapManager::Create(int minSize, int maxSize_) 
{
	minSize = roundTo(minSize, pageSize);
	maxSize = roundTo(maxSize_, pageSize);
	heapHead = ::VirtualAlloc(NULL, maxSize, MEM_RESERVE, PAGE_READWRITE);
	if( heapHead == 0 ) {
		std::cerr << "Cannot reserve virtual memory" << std::endl;
		return;
	}
	int numberOfPages = minSize / pageSize;
	commitPages(heapHead, minSize);
	// Начально выделенное количество страниц будет всегда в физической памяти до вызова Destroy
	pages.resize(numberOfPages, 1);
	addFreeBlock(heapHead, maxSize);
}

void* CHeapManager::Alloc(int size) {
	size = roundTo(size, sizeof(int));
	LPVOID blockStart = 0;
	int blockSize = 0;
	if( size + sizeof(int) < pageSize ) {
		std::make_pair(std::ref(blockStart), std::ref(blockSize)) = findEmptyBlockAndErase(freeSmallMemoryBlocks, size);
	}
	if( blockStart == 0 && size + sizeof(int) < mediumMemoryBlockMaxSize ) {
		std::make_pair(std::ref(blockStart), std::ref(blockSize)) = findEmptyBlockAndErase(freeMediumMemoryBlocks, size);
	}
	if( blockStart == 0 ) {
		std::make_pair(std::ref(blockStart), std::ref(blockSize)) = findEmptyBlockAndErase(freeLargeMemoryBlocks, size);
	}
	if( blockStart == 0 ) {
		std::cerr << "Not enough memory" << std::endl;
		return 0;
	} else {
		return commitAndUpdate(blockStart, blockSize, size);
	}
}

void CHeapManager::Free(void *mem) 
{
	mem = static_cast<byte*>(mem) - sizeof(int);
	int blockSize;
	memcpy(&blockSize, mem, sizeof(int));
	int startPage, endPage;
	std::make_pair(std::ref(startPage), std::ref(endPage)) = getStartAndEndPages(mem, blockSize);
	int firstZeroPage, lastZeroPage;
	firstZeroPage = startPage - 1;
	for( int page = startPage; page <= endPage; page++ ) {
		pages[page]--;
		if( pages[page] == 0 ) {
			if( firstZeroPage == startPage - 1 ) {
				firstZeroPage = page;
			}
			lastZeroPage = page;
		}
	}
	addEmptyBlockWithMerge(mem, blockSize + sizeof(int));
	allocatedBlocks.erase(mem);
	if( firstZeroPage >= startPage ) {
		VirtualFree(static_cast<byte*>(heapHead) + firstZeroPage * pageSize, (lastZeroPage - firstZeroPage + 1)
			* pageSize, MEM_DECOMMIT);
	}
}

void CHeapManager::Destroy() 
{
	for( auto it = allocatedBlocks.begin(); it != allocatedBlocks.end(); it++ ) {
		std::cerr << "Unfreed region: start " << *it << " size: " << getBlockSize(*it) << std::endl;
	}
	VirtualFree(heapHead, maxSize, MEM_RELEASE);
	heapHead = 0;
	pages = std::vector<int>();
	freeSmallMemoryBlocks = std::set<LPVOID>();
	freeLargeMemoryBlocks = freeMediumMemoryBlocks = std::set<std::pair<LPVOID, int>>();
}


bool CHeapManager::commitPages(LPVOID start, int size)
{
	LPVOID commited = ::VirtualAlloc(start, size, MEM_COMMIT, PAGE_READWRITE);
	if (commited == 0) {
		std::cerr << "Cannot commit physical memory" << std::endl;
		return false;
	}
	return true;
}

int CHeapManager::getPageSize() 
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwPageSize;
}

void CHeapManager::addFreeBlock(LPVOID start, int size) 
{
	if( size < pageSize ) {
		int startPage, endPage;
		std::make_pair(std::ref(startPage), std::ref(endPage)) = getStartAndEndPages(start, size);
		if (pages[startPage] == 0) {
			exit(1000);
		}
		memcpy(start, &size, sizeof(int));
		freeSmallMemoryBlocks.insert(start);
	} else if( size < mediumMemoryBlockMaxSize ) {
		freeMediumMemoryBlocks.insert(std::make_pair(start, size));
	} else {
		freeLargeMemoryBlocks.insert(std::make_pair(start, size));
	}
}

LPVOID CHeapManager::commitAndUpdate(LPVOID block, int blockSize, int requiredSize) 
{
	int startPage, endPage;
	std::make_pair(std::ref(startPage), std::ref(endPage)) = getStartAndEndPages(block, requiredSize);
	int numberOfPages = endPage - startPage + 1;
	if (endPage >= pages.size()) {
		pages.resize(endPage + 1, 0);
	}
	if( commitPages(block, requiredSize + sizeof(int)) ) {
		memcpy(block, &requiredSize, sizeof(int));
		for (int page = startPage; page <= endPage; page++) {
			pages[page] += 1;
		}
		if( requiredSize + sizeof(int) < blockSize ) {
			addFreeBlock(static_cast<byte*>(block)+sizeof(int) + requiredSize, blockSize - requiredSize - sizeof(int));
		}
		allocatedBlocks.insert(block);
		return static_cast<byte*>(block) + sizeof(int);
	} else {
		return 0;
	}
}

template <typename setType>
std::pair<LPVOID, int> CHeapManager::findEmptyBlockAndErase(std::set<setType>& targetSet, int size)
{
	for (auto it = targetSet.begin(); it != targetSet.end(); it++ ) {
		if( getBlockSize(*it) >= size + sizeof(int) ) {
			LPVOID block = getBlock(*it);
			int size = getBlockSize(*it);
			targetSet.erase(it);
			return std::make_pair(block, size);
		}
	}
	return std::make_pair(static_cast<LPVOID>(0), 0);
}

std::pair<int, int> CHeapManager::getStartAndEndPages(LPVOID block, int blockSize)
{
	int byteStartShift = static_cast<byte*>(block)-static_cast<byte*>(heapHead);
	int startPage = byteStartShift / pageSize;
	int endPage = (byteStartShift + sizeof(int) + blockSize - 1) / pageSize;
	return std::make_pair(startPage, endPage);
}

int CHeapManager::getBlockSize(LPVOID pointer) 
{
	int result;
	memcpy(&result, pointer, sizeof(int));
	return result;
}

int CHeapManager::getBlockSize(std::pair<LPVOID, int> pair) 
{
	return pair.second;
}

LPVOID CHeapManager::getBlock(LPVOID pointer) {
	return pointer;
}

LPVOID CHeapManager::getBlock(std::pair<LPVOID, int> pair) 
{
	return pair.first;
}

int CHeapManager::roundTo(int number, int divider) 
{
	return (number + divider - 1) / divider * divider;
}

void CHeapManager::addEmptyBlockWithMerge(LPVOID block, int blockSize)
{
	LPVOID leftBorder;
	int size = blockSize;
	std::pair<LPVOID, int> fromSmall, fromMedium, fromLarge, best;
	std::pair<LPVOID, int> compareElement = std::make_pair(block, blockSize);
	int leftSize;
	best = getBestMergeResult(tryLeftMerge(freeSmallMemoryBlocks, block), tryLeftMerge(freeMediumMemoryBlocks, 
		compareElement), tryLeftMerge(freeLargeMemoryBlocks, compareElement));
	if( best.first != 0 ) {
		leftBorder = best.first;
		size += best.second;
	} else {
		leftBorder = block;
	}
	best = getBestMergeResult(tryRightMerge(freeSmallMemoryBlocks, block, blockSize), tryRightMerge(freeMediumMemoryBlocks,
		compareElement, blockSize), tryRightMerge(freeLargeMemoryBlocks, compareElement, blockSize));
	if (best.first != 0) {
		size += best.second;
	}
	addFreeBlock(leftBorder, size);
}

template <typename setType>
std::pair<LPVOID, int> CHeapManager::tryLeftMerge(std::set<setType>& targetSet, setType compareElement) 
{
	std::set<setType>::iterator found = targetSet.upper_bound(compareElement);
	if ((targetSet.size() == 0) || found == targetSet.begin() || compareElement < *(--found)) {
		return std::make_pair(static_cast<LPVOID>(0), 0);
	}
	int leftSize = getBlockSize(*found);
	LPVOID leftBlock = getBlock(*found);
	LPVOID currentBlock = getBlock(compareElement);
	if( static_cast<byte*>(currentBlock) - static_cast<byte*>(leftBlock) == leftSize ) {
		targetSet.erase(found);
		return std::make_pair(leftBlock, leftSize);
	} else {
		return std::make_pair(static_cast<LPVOID>(0), 0);
	}
}

template <typename setType>
std::pair<LPVOID, int> CHeapManager::tryRightMerge(std::set<setType>& targetSet, setType compareElement, int currentSize)
{
	std::set<setType>::iterator found = targetSet.upper_bound(compareElement);
	if (found == targetSet.end()) {
		return std::make_pair(static_cast<LPVOID>(0), 0);
	}
	LPVOID currentBlock = getBlock(compareElement);
	LPVOID rightBlock = getBlock(*found);
	int rightSize = getBlockSize(*found);
	if( static_cast<byte*>(rightBlock) - static_cast<byte*>(currentBlock) == currentSize) {
		targetSet.erase(found);
		return std::make_pair(rightBlock, rightSize);
	} else {
		return std::make_pair(static_cast<LPVOID>(0), 0);
	}
}

std::pair<LPVOID, int> CHeapManager::getBestMergeResult(std::pair<LPVOID, int> first, std::pair<LPVOID, int> second,
	std::pair<LPVOID, int> third) 
{
	if (first.first != 0) {
		if (second.first != 0 || third.first != 0) {
			exit(1000);
		}
		return first;
	} else if (second.first != 0) {
		if (third.first != 0) {
			exit(1000);
		}
		return second;
	} else {
		return third;
	}
}