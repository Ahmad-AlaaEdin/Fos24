#include <inc/lib.h>

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
#define HEAP_SIZE ((USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE)
struct uh
{
	uint32 va;
	uint32 pages;
} free_list[HEAP_SIZE];
// struct spinlock heaplock;
void *sbrk(int increment)
{
	return (void *)sys_sbrk(increment);
}
bool init = 0;
//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void *malloc(uint32 size)
{
	//==============================================================
	// DON'T CHANGE THIS CODE========================================
	if (size == 0)
		return NULL;
	//==============================================================
	// TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	// panic("malloc() is not implemented yet...!!");
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE)
	{

		return alloc_block_FF(size);
	}
	else
	{
		// cprintf("##2\n");
		int ret;
		uint32 s = ((myEnv->env_limit - USER_HEAP_START) / PAGE_SIZE) + 1;
		uint32 start;
		int flag = 0;
		uint32 iterator;
		int pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;

		for (uint32 j = myEnv->env_limit + PAGE_SIZE; j < USER_HEAP_MAX; j +=
																		 PAGE_SIZE)
		{
			iterator = (j - USER_HEAP_START) / PAGE_SIZE;
			if (free_list[iterator].pages == 0)
			{
				if (flag == 0)
					start = j;

				flag++;
			}
			else
			{
				flag = 0;
			}

			if (flag == pages)
			{
				// cprintf("##3\n");
				break;
			}
		}
		if (flag == pages)
		{
			// cprintf("##4\n");
			sys_allocate_user_mem(start, pages * PAGE_SIZE);
			for (uint32 j = start; j < (start + (pages * PAGE_SIZE));
				 j += PAGE_SIZE)
			{
				iterator = (j - USER_HEAP_START) / PAGE_SIZE;
				free_list[iterator].pages = pages;
			}
			return (void *)start;
		}
	}
	return NULL;
	// Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	// to check the current strategy
}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
int idx = 1;
void free(void *virtual_address)
{
	// TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	//  Write your code here, remove the panic and write your code
	uint32 start = (uint32)virtual_address;
	uint32 pages;
	// cprintf("\nFree : %d\n", idx);
	idx++;
	if (start >= USER_HEAP_MAX && start < USER_HEAP_START)
	{

		panic("free() is not implemented yet...!!");
	}
	else if (start >= USER_HEAP_START && start < (myEnv->env_limit))
	{
		// cprintf("\nOK\n");
		free_block(virtual_address);
		return;
	}
	else if (start >= (myEnv->env_limit + PAGE_SIZE) && start < USER_HEAP_MAX)
	{
		uint32 iterator;
		iterator = (start - USER_HEAP_START) / PAGE_SIZE;
		pages = free_list[iterator].pages;
		for (uint32 j = (uint32)virtual_address; j < ((uint32)virtual_address + (pages * PAGE_SIZE));
			 j += PAGE_SIZE)
		{
			iterator = (j - USER_HEAP_START) / PAGE_SIZE;
			free_list[iterator].pages = 0;
		}
		sys_free_user_mem(start, pages * PAGE_SIZE);
	}
}

//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void *smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	// DON'T CHANGE THIS CODE========================================
	if (size == 0)
		return NULL;

	//	init_spinlock(&heaplock, "heaplock");
	//==============================================================
	// TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	// panic("smalloc() is not implemented yet...!!");
	uint32 iterator;
	uint32 start;
	uint32 flag = 0;
	int pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	// acquire_spinlock(&heaplock);
	for (uint32 j = myEnv->env_limit + PAGE_SIZE; j < USER_HEAP_MAX; j +=
																	 PAGE_SIZE)
	{
		iterator = (j - USER_HEAP_START) / PAGE_SIZE;
		if (free_list[iterator].pages == 0)
		{
			if (flag == 0)
				start = j;

			flag++;
		}
		else
		{
			flag = 0;
		}

		if (flag == pages)
		{
			// cprintf("##3\n");
			break;
		}
	}
	if (flag == pages)
	{
		// cprintf("##4\n");
		//

		int ret = sys_createSharedObject(sharedVarName, pages * PAGE_SIZE, isWritable, (void *)start);
		if (ret == E_NO_SHARE)
			return NULL;
		if (ret != E_SHARED_MEM_EXISTS)
		{
			for (uint32 j = start; j < (start + (pages * PAGE_SIZE));
				 j += PAGE_SIZE)
			{
				iterator = (j - USER_HEAP_START) / PAGE_SIZE;
				free_list[iterator].pages = pages;
			}
			return (void *)start;
		}
	}
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void *sget(int32 ownerEnvID, char *sharedVarName)
{
	// TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	//  Write your code here, remove the panic and write your code
	// panic("sget() is not implemented yet...!!");
	int size = sys_getSizeOfSharedObject(ownerEnvID, sharedVarName);
	uint32 start;
	uint32 iterator;
	uint32 flag = 0;
	// acquire_spinlock(&heaplock);
	if (size != E_SHARED_MEM_NOT_EXISTS)
	{

		int pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
		for (uint32 j = myEnv->env_limit + PAGE_SIZE; j < USER_HEAP_MAX; j +=
																		 PAGE_SIZE)
		{
			iterator = (j - USER_HEAP_START) / PAGE_SIZE;
			if (free_list[iterator].pages == 0)
			{
				if (flag == 0)
					start = j;

				flag++;
			}
			else
			{
				flag = 0;
			}

			if (flag == pages)
			{
				// cprintf("##3\n");
				break;
			}
		}
		if (flag == pages)
		{
			// cprintf("##4\n");
			//

			int ret = sys_getSharedObject(ownerEnvID, sharedVarName, (void *)start);
			if (ret == E_NO_SHARE)
				return NULL;

			for (uint32 j = start; j < (start + (pages * PAGE_SIZE));
				 j += PAGE_SIZE)
			{
				iterator = (j - USER_HEAP_START) / PAGE_SIZE;
				free_list[iterator].pages = pages;
			}
			// release_spinlock(&heaplock);
			return (void *)start;
		}
	}
	// release_spinlock(&heaplock);
	return NULL;
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void *virtual_address)
{
	// TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
	//  Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;
}

//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");
}
void shrink(uint32 newSize)
{
	panic("Not Implemented");
}
void freeHeap(void *virtual_address)
{
	panic("Not Implemented");
}
