#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

// Initialize the dynamic allocator of kernel heap with the given start address, size & limit
// All pages in the given range should be allocated
// Remember: call the initialize_dynamic_allocator(..) to complete the initialization
// Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	// TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	//  Write your code here, remove the panic and write your code
	// panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
	m_daStart = daStart;
	m_daLimit = daLimit;
	if (m_daStart + initSizeToAllocate > m_daLimit)
	{
		return E_NO_MEM;
	}
	// cprintf("NUM_OF_KHEAP_PAGES : %d", NUM_OF_KHEAP_PAGES);
	m_brk = daStart + initSizeToAllocate;

	uint32 unmappedsize = daLimit - m_brk;
	init_spinlock(&lck, "heap lock");
	for (int i = daStart; i < m_brk; i += PAGE_SIZE)
	{
		struct FrameInfo *ptr_frame_info;
		int info = allocate_frame(&ptr_frame_info);
		map_frame(ptr_page_directory, ptr_frame_info, i, PERM_USER | PERM_WRITEABLE);
		ptr_frame_info->va = i;
		ptr_frame_info->bufferedVA = i;
	}
	initialize_dynamic_allocator(daStart, initSizeToAllocate);
	return 0;
}

void *sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	// MS2: COMMENT THIS LINE BEFORE START CODING==========
	// return (void*)-1 ;
	//====================================================

	// TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	//  Write your code here, remove the panic and write your code
	// cprintf("\nsbrk SYS\n");
	if (numOfPages == 0)
		return (void *)m_brk;

	uint32 total = (numOfPages * PAGE_SIZE) + m_brk;

	uint32 returenedBrk = m_brk;
	if (numOfPages == 0)
		return (void *)m_brk;
	else if (total > m_daLimit)
	{
		return (void *)-1;
	}
	else if (numOfPages > 0)
	{
		m_brk = total;

		struct FrameInfo *frame;
		int ret;
		for (uint32 i = returenedBrk; i < m_brk; i += PAGE_SIZE)
		{
			ret = allocate_frame(&frame);
			if (ret == E_NO_MEM)
			{
				return (void *)-1;
			}

			ret = map_frame(ptr_page_directory, frame, i, PERM_WRITEABLE | PERM_USER);
			if (ret == E_NO_MEM)
			{
				return (void *)-1;
			}
			frame->va = i;
		}
		return (void *)returenedBrk;
	}
	return (void *)-1;
}

// TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void *kmalloc(unsigned int size)
{
	// TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
	//  Write your code here, remove the panic and write your code
	// kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	acquire_spinlock(&(lck));
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE)
	{
		// cprintf("##1\n");
		if (isKHeapPlacementStrategyFIRSTFIT())
		{

			void *add = alloc_block_FF(size);
			release_spinlock(&lck);
			return add;
		}

		if (isKHeapPlacementStrategyBESTFIT())
			return alloc_block_BF(size);
	}
	else
	{
		// cprintf("##2\n");
		int ret;
		uint32 start;
		int flag = 0;
		struct FrameInfo *frame = NULL;
		uint32 *ptr_page_table = NULL;
		int pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;

		for (uint32 j = m_daLimit + PAGE_SIZE; j < KERNEL_HEAP_MAX; j +=
																	PAGE_SIZE)
		{
			frame = get_frame_info(ptr_page_directory, j, &ptr_page_table);
			if (frame == NULL)
			{
				if (flag == 0)
					start = j;

				flag++;
			}
			else
			{
				flag = 0;
				j += (((frame->pages - 1) * PAGE_SIZE));
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
			for (uint32 j = start; j < (start + (pages * PAGE_SIZE));
				 j += PAGE_SIZE)
			{
				ret = allocate_frame(&frame);
				if (ret == E_NO_MEM)
				{
					release_spinlock(&lck);
					return (void *)-1;
				}
				frame->va = j;
				frame->pages = pages;
				ret = map_frame(ptr_page_directory, frame, j,
								PERM_PRESENT | PERM_WRITEABLE);
				if (ret == E_NO_MEM)
				{
					release_spinlock(&lck);
					return (void *)-1;
				}
			}
			if (start >= KERNEL_HEAP_START && start < m_daLimit)
			{
				// cprintf("\n DYNALLOC\n");
			}
			else if (start >= (m_daLimit + PAGE_SIZE) && start < KERNEL_HEAP_MAX)
			{

				// cprintf("\n KERNEL HEAP\n");
			}
			release_spinlock(&lck);
			return (void *)start;
		}
	}

	// change this "return" according to your answer
	// kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	release_spinlock(&lck);
	return (void *)NULL;
}

void kfree(void *virtual_address)
{
	// TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	//  Write your code here, remove the panic and write your code
	// panic("kfree() is not implemented yet...!!");

	// you need to get the size of the given allocation using its address
	// refer to the project presentation and documentation for details
	acquire_spinlock(&(lck));
	if (virtual_address >= (void *)KERNEL_HEAP_START && virtual_address < (void *)m_daLimit)
	{
		free_block(virtual_address);
	}
	else if (virtual_address >= (void *)(m_daLimit + PAGE_SIZE) && virtual_address < (void *)KERNEL_HEAP_MAX)
	{
		struct FrameInfo *frame = NULL;
		uint32 *ptr = NULL;
		frame = get_frame_info(ptr_page_directory, (uint32)virtual_address, &ptr);
		int pages = frame->pages;
		uint32 start = frame->va;

		for (uint32 i = start; i < start + (pages * PAGE_SIZE);
			 i += PAGE_SIZE)
		{

			frame = get_frame_info(ptr_page_directory, i, &ptr);
			frame->va = 0;
			frame->pages = 0;
			unmap_frame(ptr_page_directory, i);
			uint32 *ptrPT;
		}
	}
	else
	{
		release_spinlock(&lck);
		panic("Invalid Address");
	}
	release_spinlock(&lck);
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	// TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	//  Write your code here, remove the panic and write your code
	// panic("kheap_physical_address() is not implemented yet...!!");

	// return the physical address corresponding to given virtual_address
	// refer to the project presentation and documentation for details

	// EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
	uint32 ptx = PTX(virtual_address);
	uint32 offset = virtual_address & 0x00000FFF;

	uint32 *ptr_page_table = NULL;
	int ret = get_page_table(ptr_page_directory, virtual_address, &ptr_page_table);
	uint32 off1 = (((ptr_page_table[ptx] >> 12) * PAGE_SIZE) + offset) % PAGE_SIZE;

	if (ret == TABLE_IN_MEMORY && ptr_page_table != NULL && ptr_page_table[ptx] & PERM_PRESENT)
	{
		struct FrameInfo *frame = to_frame_info(((ptr_page_table[ptx] >> 12) * PAGE_SIZE) + offset);
		unsigned int off2 = (((ptr_page_table[ptx] >> 12) * PAGE_SIZE) + offset) << 20;
		return (((ptr_page_table[ptx] >> 12) * PAGE_SIZE) + offset);
	}
	return 0;
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	// TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	//  Write your code here, remove the panic and write your code
	// panic("kheap_virtual_address() is not implemented yet...!!");

	// return the virtual address corresponding to given physical_address
	// refer to the project presentation and documentation for details

	// EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================

	struct FrameInfo *frame = to_frame_info(physical_address);

	uint32 offset = physical_address % PAGE_SIZE;
	unsigned int frame_off = frame->va + (offset);
	if (frame_off == (offset >> 20) || frame->va == 0)
	{
		return 0;
	}
	return frame_off;
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	// TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	//  Write your code here, remove the panic and write your code
	// return NULL;
	// panic("krealloc() is not implemented yet...!!");
	return NULL;

	if (virtual_address == NULL)
		return kmalloc(new_size);

	if (new_size == 0)
	{
		kfree(virtual_address);
		return NULL;
	}

	// reallocation in block allocator region
	bool blockAllocated = (uint32)virtual_address >= KERNEL_HEAP_START && (uint32)virtual_address < m_daLimit;
	if (blockAllocated && new_size <= DYN_ALLOC_MAX_BLOCK_SIZE)
	{
		return realloc_block_FF(virtual_address, new_size);
	}

	// reallocation from block allocator to page allocator and vice versa
	if (blockAllocated != (new_size <= DYN_ALLOC_MAX_BLOCK_SIZE))
	{
		kfree(virtual_address);
		return kmalloc(new_size);
	}

	// reallocation in page allocator region

	int pageIndex = ((int)virtual_address - m_daLimit - PAGE_SIZE) / PAGE_SIZE;
	new_size = ROUNDUP(new_size, PAGE_SIZE);
	if (chunkSZ[pageIndex] == new_size)
	{
		return virtual_address;
	}

	if (chunkSZ[pageIndex] > new_size)
	{
		// need to unmap extra pages
		uint32 curVA = (uint32)virtual_address + chunkSZ[pageIndex] - PAGE_SIZE;
		int extraPages = (chunkSZ[pageIndex] - new_size) / PAGE_SIZE;
		while (extraPages--)
		{
			uint32 *ptrPT = NULL;
			struct FrameInfo *frame = get_frame_info(ptr_page_directory, curVA, &ptrPT);
			unmap_frame(ptr_page_directory, curVA);

			curVA -= PAGE_SIZE;
		}

		chunkSZ[pageIndex] = new_size;
		return virtual_address;
	}

	// new size > prev size
	// count free pages next
	int nxtFree = 0;
	uint32 curVA = (uint32)virtual_address + chunkSZ[pageIndex], *ptrPT = NULL;
	while (curVA < KERNEL_HEAP_MAX)
	{
		if (get_frame_info(ptr_page_directory, curVA, &ptrPT) != 0)
			break;

		nxtFree++;
		curVA += PAGE_SIZE;
	}

	int extraPages = (new_size - chunkSZ[pageIndex]) / PAGE_SIZE;
	// if enough next free pages
	if (nxtFree >= extraPages)
	{
		curVA = (uint32)virtual_address + chunkSZ[pageIndex];
		int startVA = curVA;
		int startIndex = (startVA - m_daLimit - PAGE_SIZE) / PAGE_SIZE;
		chunkSZ[startIndex] = 0;
		while (extraPages--)
		{
			struct FrameInfo *frame = NULL;
			int ret = allocate_frame(&frame);
			if (ret == E_NO_MEM)
			{
				// free previously allocated pages
				kfree((void *)startVA);
				return NULL;
			}

			chunkSZ[startIndex] += PAGE_SIZE;
			frame->bufferedVA = curVA;
			ret = map_frame(ptr_page_directory, frame, curVA, PERM_WRITEABLE);
			if (ret == E_NO_MEM)
			{
				// free previously allocated pages
				kfree((void *)startVA);
				return NULL;
			}
			curVA += PAGE_SIZE;
		}

		// success
		chunkSZ[pageIndex] += chunkSZ[startIndex];
		chunkSZ[startIndex] = 0;
		return virtual_address;
	}

	// need to reallocate elsewhere
	void *ret = kmalloc(new_size);
	if (ret != NULL)
		kfree(virtual_address);

	return ret;
}
