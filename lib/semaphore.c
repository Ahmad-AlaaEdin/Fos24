// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	// TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("create_semaphore is not implemented yet");
	// Your Code is Here...
	struct semaphore sem;

	sem.semdata = smalloc(semaphoreName, sizeof(struct __semdata), 1);
	sem.semdata->count = value;
	sem.semdata->lock = 0;
	strncpy(sem.semdata->name, semaphoreName, 64);
	sys_init_queue(&(sem.semdata->queue));

	return sem;
}
struct semaphore get_semaphore(int32 ownerEnvID, char *semaphoreName)
{
	// TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	// COMMENT THE FOLLOWING LINE BEFORE START CODING
	// panic("get_semaphore is not implemented yet");
	// Your Code is Here...
	struct semaphore s;
	struct __semdata *semdata = (struct __semdata *)sget(ownerEnvID, semaphoreName);
	s.semdata = semdata;
	return s;
}

void wait_semaphore(struct semaphore sem)
{
	// cprintf("wait\n");

	while (xchg(&(sem.semdata->lock), 1) != 0)
		;
	// cprintf("after wait\n");

	sem.semdata->count--;

	if (sem.semdata->count < 0)
	{
		// cprintf("block\n");
		sys_dequeue((sem.semdata));
	}
	else
	{

		sem.semdata->lock = 0;
	}
}

void signal_semaphore(struct semaphore sem)
{
	// cprintf("signal1\n");

	while (xchg(&(sem.semdata->lock), 1) != 0)
		;
	// cprintf("signal2\n");

	sem.semdata->count++;

	if (sem.semdata->count <= 0)
	{
		// cprintf("wake\n");
		sys_enqueue((sem.semdata));
	}

	sem.semdata->lock = 0;
}

int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}