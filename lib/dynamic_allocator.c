/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void *va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1);
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void *va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1);
	return (~(*curBlkMetaData) & 0x1);
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		// cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement *blk;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(Address :%x size: %d, isFree: %d)\n", (uint32 *)blk, get_block_size(blk), is_free_block(blk));
	}
	cprintf("=========================================\n");
	/*
		cprintf("=========================================\n");
		struct BlockElement *blk;
		cprintf("\nDynAlloc Blocks List:\n");
		LIST_FOREACH(blk, &list)
		{
			cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk));
		}
		cprintf("=========================================\n");*/
}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{

	//==================================================================================
	// DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0)
			initSizeOfAllocatedSpace++; // ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return;
		is_initialized = 1;
	}
	// cprintf("initSizeOfAllocatedSpace%d", initSizeOfAllocatedSpace);
	//==================================================================================
	//==================================================================================

	// TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("initialize_dynamic_allocator is not implemented yet");
	uint32 *beg = (uint32 *)daStart;
	uint32 *end = (uint32 *)(daStart + initSizeOfAllocatedSpace - sizeof(uint32));
	*beg = *end = 1;
	uint32 *header = (uint32 *)(daStart + sizeof(uint32));
	uint32 *footer = (uint32 *)(daStart + initSizeOfAllocatedSpace - 2 * sizeof(uint32));
	*header = *footer = initSizeOfAllocatedSpace - (2 * sizeof(uint32));

	LIST_INIT(&freeBlocksList);
	struct BlockElement *newBlock = (struct BlockElement *)(daStart + 8);
	LIST_INSERT_HEAD(&freeBlocksList, newBlock);

	// Reallocate Test
	/*
	print_blocks_list(freeBlocksList);
	if (realloc_block_FF(NULL, 1024) != header + 1)
		cprintf("1 Is Wrong\n");
	print_blocks_list(freeBlocksList);
	if (realloc_block_FF(header + 1, 0) != NULL)
		cprintf("2 Is Wrong\n");
	print_blocks_list(freeBlocksList);
	if (realloc_block_FF(NULL, 0) != NULL)
		cprintf("3 Is Wrong\n");
	print_blocks_list(freeBlocksList);
	realloc_block_FF(NULL, 1024);
	print_blocks_list(freeBlocksList);
	if (realloc_block_FF(header + 1, initSizeOfAllocatedSpace + 123456) != NULL)
		cprintf("4 Is Wrong\n");
	print_blocks_list(freeBlocksList);*/
}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void *va, uint32 totalSize, bool isAllocated)
{
	// TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("set_block_data is not implemented yet");
	// cprintf("va %p\n", (void *)va);
	// cprintf("totalSize %d\n", totalSize);
	uint32 *header = (uint32 *)((uint8 *)va - sizeof(uint32));
	uint32 *footer = (uint32 *)((uint8 *)va + totalSize - 8);

	*header = *footer = totalSize + isAllocated;
	// cprintf("header %p\n", *header);
	// cprintf("footer %p\n", *footer);

	// cprintf("header l %p\n", header);
	// cprintf("footer l %p\n", footer);
}

//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	// DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0)
			size++; // ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE;
		if (!is_initialized)
		{
			uint32 required_size = size + 2 * sizeof(int) /*header & footer*/ + 2 * sizeof(int) /*da begin & end*/;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE) / PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	// TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("alloc_block_FF is not implemented yet");
	uint32 totalSize = size + 2 * sizeof(uint32);
	struct BlockElement *iterator;
	uint32 *blockSize;

	// cprintf("	size:%p \n\n", totalSize);

	LIST_FOREACH(iterator, &freeBlocksList)
	{

		blockSize = ((uint32 *)iterator - 1);
		// cprintf("	*blockSize:%d \n\n", *blockSize);
		if (*blockSize % 2 == 1 || *blockSize < totalSize)
			continue;
		if (*blockSize % 2 == 0 && *blockSize == totalSize)
		{
			// cprintf("11\n");
			set_block_data(iterator, *blockSize, 1);
			LIST_REMOVE(&freeBlocksList, iterator);
			return iterator;
		}
		if (*blockSize % 2 == 0 && (*blockSize - totalSize <= 16 && *blockSize - totalSize >= 0))
		{
			// cprintf("22\n");
			set_block_data(iterator, *blockSize, 1);
			LIST_REMOVE(&freeBlocksList, iterator);
			return iterator;
		}

		if (*blockSize >= totalSize && *blockSize <= totalSize + 2 * sizeof(uint32))
		{
			// cprintf("	1: \n\n");
			set_block_data(iterator, *blockSize, 1);
			return iterator;
		}
		else
		{
			// cprintf("	2: \n\n");
			uint32 newSize = *blockSize - totalSize;
			uint8 *newPointer = (uint8 *)iterator + totalSize;
			struct BlockElement *newBlock = (struct BlockElement *)newPointer;
			set_block_data(iterator, totalSize, 1);
			set_block_data(newPointer, newSize, 0);
			/*////cprintf("	iterator:%p \n\n", iterator);
			////cprintf("	iterator:%p \n\n", newPointer);
			////cprintf("	newSize:%d \n\n", newSize);
			////cprintf("	(uint8 *)iterator + totalSize + sizeof(uint32):%p \n\n", (uint8 *)iterator + totalSize + sizeof(uint32));
			////cprintf("	totalSize:%d \n\n", totalSize);
			////cprintf("	remainingSize:%d \n\n", *blockSize - (totalSize / sizeof(uint32)));
			////cprintf("	newBlock:%p \n\n", (uint32 *)newBlock);*/
			LIST_INSERT_AFTER(&freeBlocksList, iterator, newBlock);
			LIST_REMOVE(&freeBlocksList, iterator);

			return iterator;
		}
	}
	return NULL;
}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	// TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("alloc_block_BF is not implemented yet");
	// Your Code is Here...
	if (size % 2 != 0)
		size++; // ensure that the size is even (to use LSB as allocation flag)
	if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
		size = DYN_ALLOC_MIN_BLOCK_SIZE;
	uint32 totalSize = size + 8;
	struct BlockElement *iterator;
	struct BlockElement *curBlock = NULL;
	uint32 curSize = DYN_ALLOC_MAX_SIZE;

	LIST_FOREACH(iterator, &freeBlocksList)
	{
		uint32 iteratorSize = get_block_size(iterator);
		if (iteratorSize == totalSize)
		{
			// cprintf("##1##\n");
			set_block_data(iterator, totalSize, 1);
			LIST_REMOVE(&freeBlocksList, iterator);
			return iterator;
		}
		if ((int)(iteratorSize - totalSize) > 0 && iteratorSize < curSize)
		{
			// cprintf("##2##\n");
			// cprintf("	iteratorSize%d \n\n", iteratorSize);
			// cprintf("	totalSize:%d \n\n", totalSize);
			curBlock = iterator;
			curSize = iteratorSize;
		}
	}
	// cprintf("	curBlock:%p \n\n", curBlock);
	//   ////cprintf("	iterator:%p \n\n", newPointer);
	// cprintf("	get_block_size(iterator):%d \n\n", get_block_size(curBlock));
	// print_blocks_list(freeBlocksList);
	if (curBlock != NULL)
	{
		if (curSize >= totalSize && curSize - totalSize <= 16)
		{
			// cprintf("	1: \n\n");
			set_block_data(curBlock, get_block_size(curBlock), 1);
			LIST_REMOVE(&freeBlocksList, curBlock);
			return curBlock;
		}
		else
		{
			// cprintf("	2: \n\n");

			uint32 newSize = curSize - totalSize;
			uint8 *newPointer = (uint8 *)curBlock + totalSize;
			struct BlockElement *newBlock = (struct BlockElement *)newPointer;
			set_block_data(curBlock, totalSize, 1);
			set_block_data(newPointer, newSize, 0);

			LIST_INSERT_AFTER(&freeBlocksList, curBlock, newBlock);

			LIST_REMOVE(&freeBlocksList, curBlock);

			return curBlock;
		}
	}
	// cprintf("##3##\n");
	return NULL;
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	// TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("free_block is not implemented yet");
	uint32 blockSize = get_block_size(va);
	struct BlockElement *curBlock = (struct BlockElement *)va;
	struct BlockElement *nextBlock = NULL;
	struct BlockElement *prevBlock = NULL;
	uint32 nextSize = *((uint32 *)((uint8 *)va + blockSize - sizeof(uint32)));
	uint32 prevSize = *((uint32 *)((uint8 *)va - 2 * sizeof(uint32)));
	// cprintf("va:%p\n", va);
	// cprintf("size:%d\n", blockSize);
	// cprintf("next:%p\n", (uint32 *)((uint8 *)va + blockSize - sizeof(uint32)));
	// cprintf("isFree:%d\n", *((uint32 *)((uint8 *)va + blockSize - sizeof(uint32))));
	// cprintf("prev:%p\n", (uint32 *)((uint8 *)va - 2 * sizeof(uint32)));
	// cprintf("isFree:%d\n", *((uint32 *)((uint8 *)va - 2 * sizeof(uint32))));

	if (prevSize % 2 == 0)
		prevBlock = (struct BlockElement *)(uint32 *)((uint8 *)va - prevSize);
	if (nextSize % 2 == 0)
		nextBlock = (struct BlockElement *)(uint32 *)((uint8 *)va + blockSize);

	if (prevSize % 2 == 0 && nextSize % 2 == 0)
	{
		uint32 newSize = blockSize + prevSize + nextSize;
		set_block_data((uint32 *)prevBlock, newSize, 0);
		LIST_REMOVE(&freeBlocksList, nextBlock);
		return;
	}

	if (prevSize % 2 == 0)
	{

		uint32 newSize = blockSize + prevSize;
		set_block_data((uint32 *)prevBlock, newSize, 0);
		return;
	}
	if (nextSize % 2 == 0)
	{
		// cprintf("hereFree /n");

		uint32 newSize = blockSize + nextSize;
		struct BlockElement *newBlock = (struct BlockElement *)(uint32 *)va;
		LIST_INSERT_BEFORE(&freeBlocksList, nextBlock, curBlock);
		LIST_REMOVE(&freeBlocksList, nextBlock);
		set_block_data((uint32 *)va, newSize, 0);
		return;
	}
	set_block_data(va, blockSize, 0);
	struct BlockElement *id;
	int idx = 0;
	struct BlockElement *iterator;
	if (LIST_EMPTY(&freeBlocksList))
	{
		LIST_INSERT_HEAD(&freeBlocksList, curBlock);
	}
	else
	{
		bool inserted = 0;
		LIST_FOREACH(iterator, &freeBlocksList)
		{

			if ((uint32 *)curBlock < (uint32 *)iterator)
			{
				LIST_INSERT_BEFORE(&freeBlocksList, iterator, curBlock);
				inserted = 1;
				break;
			}
		}
		if (inserted == 0)
			LIST_INSERT_TAIL(&freeBlocksList, curBlock);
	}

	// print_blocks_list(freeBlocksList);
}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void *va, uint32 new_size)
{
	// TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("realloc_block_FF is not implemented yet");
	if (new_size == 0 && va != NULL)
	{
		free_block(va);
		return NULL;
	}
	if (new_size == 0)
		return NULL;

	if (va == NULL)
		return alloc_block_FF(new_size);
	struct BlockElement *curBlock = (struct BlockElement *)va;
	uint32 blockSize = get_block_size(va) - 8;
	uint32 nextSize = *((uint32 *)((uint8 *)va + get_block_size(va) - sizeof(uint32)));
	uint32 afterNextSize = *((uint32 *)((uint8 *)va + (blockSize + 4) + nextSize));
	// cprintf("blockSize : %d", blockSize);
	// cprintf(" new_size: %d", new_size);
	// cprintf("nextSize : %d", nextSize);
	// cprintf("afterNextSize : %d\n", afterNextSize);
	// cprintf("cur : %x\n", va);
	// cprintf("next : %x\n", (uint32 *)((uint8 *)va + get_block_size(va) - sizeof(uint32)));
	struct BlockElement *nextBlock = NULL;
	if (nextSize % 2 == 0)
		nextBlock = (struct BlockElement *)(uint32 *)((uint8 *)va + get_block_size(va));
	// Same Size
	if (new_size == (blockSize))
		return va;
	// Smaller Size

	if (new_size < (blockSize))
	{
		if (blockSize - new_size < 16)
			return va;
		// cprintf("here 3:  \n");
		//    Split
		uint32 totalSize = (blockSize + 8) - new_size;
		uint8 *newPointer = (uint8 *)curBlock + new_size + 8;
		struct BlockElement *newBlock = (struct BlockElement *)newPointer;
		set_block_data(va, new_size + 8, 1);
		set_block_data(newPointer, blockSize - (new_size), 1);
		free_block(newPointer);
		return va;
	}

	// greater Size

	uint32 neededSize = new_size - (blockSize);
	// Next Is Free
	if (nextSize % 2 == 0)
	{
		if (nextSize - (neededSize) < 16)
		{
			// cprintf("here :  \n");
			set_block_data(va, (nextSize + blockSize + 8), 1);
			LIST_REMOVE(&freeBlocksList, nextBlock);
			return va;
		}
		else if ((int)(nextSize - neededSize) >= 0 && (neededSize) >= 0)
		{
			// cprintf("here2 :  \n");
			uint8 *newPointer = (uint8 *)nextBlock + neededSize;
			struct BlockElement *newBlock = (struct BlockElement *)newPointer;
			set_block_data(va, (neededSize + blockSize + 8), 1);
			set_block_data(newPointer, nextSize - neededSize, 0);

			LIST_INSERT_AFTER(&freeBlocksList, nextBlock, newBlock);

			LIST_REMOVE(&freeBlocksList, nextBlock);
			return va;
		}
	}

	void *address = alloc_block_FF(new_size);
	if (address != NULL)
	{
		memcpy(address, va, blockSize);
		free_block(va);
		return address;
	}
	return NULL;
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================س
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}
