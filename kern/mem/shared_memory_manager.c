#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share *get_share(int32 ownerID, char *name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
// Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list);
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char *shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share *ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
inline struct FrameInfo **create_frames_storage(int numOfFrames)
{
	// TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_frames_storage()
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("create_frames_storage is not implemented yet");
	// Your Code is Here...
	struct FrameInfo **frames = (struct FrameInfo **)kmalloc(numOfFrames * sizeof(struct FrameInfo *));

	if (frames != NULL)

	{
		for (int i = 0; i < numOfFrames; i++)
		{
			frames[i] = 0;
		}

		return frames;
	}

	return NULL;
}

//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
// Allocates a new shared object and initialize its member
// It dynamically creates the "framesStorage"
// Return: allocatedObject (pointer to struct Share) passed by reference
struct Share *create_share(int32 ownerID, char *shareName, uint32 size, uint8 isWritable)
{
	// TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("create_share is not implemented yet");
	// Your Code is Here...
	int frames = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	struct Share *share = kmalloc(sizeof(struct Share));
	struct Share *s;
	if (share != NULL)
	{
		struct FrameInfo **storage = create_frames_storage(frames);
		if (storage == NULL)
		{
			kfree(share);
			return NULL;
		}
		share->references = 1;
		share->ID = (uint32)share & 0x7FFFFFFF;
		share->isWritable = isWritable;
		share->ownerID = ownerID;
		strncpy(share->name, shareName, 64);
		share->framesStorage = storage;
		share->size = (int)size;
		/*for (uint32 i = 0; i < frames; i++)
		{
			//cprintf("\npointer %x \n", share->framesStorage[i]);
		}*/
		return share;
	}
	return NULL;
}

//=============================
// [3] Search for Share Object:
//=============================
// Search for the given shared object in the "shares_list"
// Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share *get_share(int32 ownerID, char *name)
{
	// TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("get_share is not implemented yet");
	// Your Code is Here...
	acquire_spinlock(&(AllShares.shareslock));
	struct Share *i = NULL;
	LIST_FOREACH(i, &AllShares.shares_list)
	{
		if (i->ownerID == ownerID && strcmp(i->name, name) == 0)
		{
			// cprintf("\n\found\n");
			release_spinlock(&(AllShares.shareslock));
			return i;
		}
	}
	// cprintf("\nNot found\n");
	release_spinlock(&(AllShares.shareslock));
	return NULL;
}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char *shareName, uint32 size, uint8 isWritable, void *virtual_address)
{
	// TODO: [PROJECT'24.MS2 - #19] [4] SHARED MEMORY [KERNEL SIDE] - createSharedObject()
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("createSharedObject is not implemented yet");
	// Your Code is Here...
	if (get_share(ownerID, shareName) != NULL)
	{
		// cprintf("\n##0##\n");
		return E_SHARED_MEM_EXISTS;
	}
	struct Env *myenv = get_cpu_proc(); // The calling environment
	struct Share *share = create_share(ownerID, shareName, size, isWritable);
	if (share == NULL)
	{
		// cprintf("\n##1##\n");
		return E_NO_SHARE;
	}

	int np = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	struct FrameInfo *frame;
	int ret;

	acquire_spinlock(&(AllShares.shareslock));
	LIST_INSERT_TAIL(&(AllShares.shares_list), share);

	for (uint32 i = 0; i < np; i++)
	{
		ret = allocate_frame(&frame);
		if (ret == E_NO_MEM)
		{
			// cprintf("\n##2##\n");
			release_spinlock(&(AllShares.shareslock));
			return E_NO_SHARE;
		}

		ret = map_frame(myenv->env_page_directory, frame, (uint32)virtual_address, PERM_WRITEABLE | PERM_USER);
		if (ret == E_NO_MEM)
		{
			release_spinlock(&(AllShares.shareslock));
			// cprintf("\n##3##\n");
			return E_NO_SHARE;
		}
		int perms = pt_get_page_permissions(myenv->env_page_directory, (uint32)virtual_address);
		// uint32 *int_ptr = (uint32 *)virtual_address; // Cast the void* to an int*
		// uint32 value = *int_ptr;
		// cprintf("\npointer %x virtual_address  %x   writeable %d    available     %d value %d\n", frame, (uint32)virtual_address, perms & PERM_WRITEABLE, perms & PERM_AVAILABLE, value);
		virtual_address += PAGE_SIZE;
		share->framesStorage[i] = frame;
	}
	release_spinlock(&(AllShares.shareslock));
	return share->ID;
}

//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char *shareName, void *virtual_address)
{
	// TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("getSharedObject is not implemented yet");
	// Your Code is Here...

	struct Env *myenv = get_cpu_proc(); // The calling environment
	// acquire_spinlock(&(AllShares.shareslock));
	struct Share *ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	int np = ROUNDUP(ptr_share->size, PAGE_SIZE) / PAGE_SIZE;
	struct FrameInfo *frame;
	int ret;
	acquire_spinlock(&(AllShares.shareslock));
	uint32 perm = ptr_share->isWritable ? PERM_WRITEABLE : 0;
	for (uint32 i = 0; i < np; i++)
	{

		ret = map_frame(myenv->env_page_directory, ptr_share->framesStorage[i], (uint32)virtual_address, perm | PERM_USER | PERM_PRESENT);
		if (ret == E_NO_MEM)
		{
			release_spinlock(&(AllShares.shareslock));
			return E_NO_SHARE;
		}
		int perms = pt_get_page_permissions(myenv->env_page_directory, (uint32)virtual_address);
		uint32 *int_ptr = (uint32 *)virtual_address; // Cast the void* to an int*
		uint32 value = *int_ptr;
		// cprintf("\frame %x virtual_address  %x   writeable %d    available     %d value %d\n", ptr_share->framesStorage[i], (uint32)virtual_address, perms & PERM_WRITEABLE, perms & PERM_AVAILABLE, value);
		virtual_address += PAGE_SIZE;
	}
	ptr_share->references += 1;
	release_spinlock(&(AllShares.shareslock));
	return ptr_share->ID;
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
// delete the given shared object from the "shares_list"
// it should free its framesStorage and the share object itself
void free_share(struct Share *ptrShare)
{
	// TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - free_share()
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("free_share is not implemented yet");
	// Your Code is Here...
}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	// TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("freeSharedObject is not implemented yet");
	// Your Code is Here...
}
