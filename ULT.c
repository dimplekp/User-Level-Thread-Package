#include <assert.h>
#include <stdio.h>

/* We want the extra information from these definitions */
#ifndef __USE_GNU
#define __USE_GNU
#endif /* __USE_GNU */
#include <stdlib.h>
#include <ucontext.h>
#include "ULT.h"
#include "interrupt.h"

Tid ULT_DestroyThread(Tid tid);
int isValidTid(Tid toTest);
int getTid();
void stub (void (*fn)(void *), void*arg);

int threadCount;
int isFirstThread = 1;

int getTid(){
	int i;
	for(i = 1; i < ULT_MAX_THREADS; i++){
		if(!isValidTid(i))
			return i;
	}
	return ULT_MAX_THREADS;
}

int isValidTid(Tid toTest){
	int i;
	for(i=0; i<threadCount; i++)
	{
		if(readyQueue[i]->threadID == toTest)
			return 1;
	}
	return 0;
}

ThrdCtlBlk *removeFromReady(Tid thread)
{
	int i;
	ThrdCtlBlk *b;
	for(i=0; i<threadCount; i++)
	{
		if( (readyQueue[i]->threadID) == thread){
			b = readyQueue[i];
			break;
		}
	}
	for(;i<threadCount - 1; i++){
		readyQueue[i] = readyQueue[i+1];
	}
	readyQueue[threadCount-1] = NULL;
	threadCount--;
	return b;
}

ThrdCtlBlk *popThread()
{
	ThrdCtlBlk *b = readyQueue[0];
	int i;
	for(i = 0; i<threadCount - 1; i++){
		readyQueue[i] = readyQueue[i+1];
	}
	readyQueue[threadCount-1] = NULL;
	threadCount--;
	return b;
}

int initializeFirstThread()
{
  isFirstThread = 0;
	runningThread = (ThrdCtlBlk *)malloc(sizeof(ThrdCtlBlk));
	ThrdCtlBlk *b = (ThrdCtlBlk *)malloc(sizeof(ThrdCtlBlk));
	ucontext_t current;
	getcontext(&current);
	
	b->context = (ucontext_t *)malloc(sizeof(ucontext_t));
	b->context = &current;
	
	b->threadID = 0;
	threadCount = 0;
	runningThread = b;
	
	return 1;
}

Tid ULT_CreateThread(void (*fn)(void *), void *parg)
{
	// printf("IN CREATE ......\n");
	// interruptsOff();

	if(isFirstThread)
		initializeFirstThread();

	if(threadCount + 1 == ULT_MAX_THREADS)
		return ULT_NOMORE;

	ucontext_t *mycontext = (ucontext_t *)malloc(sizeof(ucontext_t));
	getcontext(mycontext);

	// Change program counter to point to stub
	mycontext->uc_mcontext.gregs[REG_EIP] = (unsigned int)&stub;

	// Allocate and initialize a stack
	unsigned int *stackPointer = malloc(ULT_MIN_STACK*sizeof(unsigned int));
	stackPointer[ULT_MIN_STACK-1] = (unsigned int)parg;
	stackPointer[ULT_MIN_STACK-2] = (unsigned int)fn;

	// Change the stack pointer to point to the top of the new stack
	mycontext->uc_mcontext.gregs[REG_ESP] = (unsigned int)&stackPointer[ULT_MIN_STACK - 3];

	ThrdCtlBlk *newThread = malloc(sizeof(ThrdCtlBlk));
	newThread->context = mycontext;
	newThread->threadID = getTid();

	readyQueue[threadCount] = newThread;
	threadCount++;

	// interruptsOn();

	return newThread->threadID;
}


Tid ULT_Yield(Tid wantTid)
{
	// printf("YIELDING ......\n");
	// interruptsOff();
	// Success - ULT_ANY(any from the ready queue), ULT_SELF(caller)
  // Failure - ULT_INVALID(tid does not correspond to a valid thread),
  //                     ULT_NONE(there are no more threads (other than the caller) available to run (in response to a call with tid set to ULT_SELF)
  //                      or available to destroy (in response to a call wtih tid set to ULT_ANY.))
	
	if(isFirstThread)
		initializeFirstThread();

	if((wantTid == ULT_SELF || wantTid == ULT_ANY) && threadCount == 0)
		return ULT_NONE;

	if(wantTid == ULT_SELF)
		return runningThread->threadID;

	if(wantTid == ULT_ANY){
	  
		int flag = 0;
		ThrdCtlBlk *nextThread;
		nextThread = popThread();
		getcontext(runningThread->context);
		if(flag == 0){
			readyQueue[threadCount] = runningThread;
			threadCount++;
			flag = 1;
			runningThread = nextThread;
			setcontext(runningThread->context);
		}

		return nextThread->threadID;
	}

	if(wantTid == runningThread->threadID)
		return runningThread->threadID; 

	if(!isValidTid(wantTid))
		return ULT_INVALID;

	// If any of the conditions didn't match
  // yield the wantTid thread
	int flag = 0;
	ThrdCtlBlk *nextThread;
	nextThread = removeFromReady(wantTid);
	ucontext_t *running = (ucontext_t *)malloc(sizeof(ucontext_t));
	getcontext(running);
	runningThread->context = running;
	if( flag == 0){
		readyQueue[threadCount] = runningThread;
		threadCount++;
		flag = 1;
		runningThread = nextThread;
		setcontext(runningThread->context);
	}
	// interruptsOn();
	return nextThread->threadID;
}

// param tid is the Tid of the thread to be destroyed.
// returns the Tid of the thread that was destroyed.
Tid ULT_DestroyThread(Tid tid)
{ 
	// printf("ready queue size : %d\n", threadCount);
	// interruptsOff();
	// Possible arguments - tid, ULT_ANY(destroy any thread except the caller),
  //                                             ULT_SELF(destroy the caller; The function obviously does not return in this case. Instead, some other ready thread gets scheduled.)
  // Success - TID
  // Failure - ULT_INVALID(tid does not correspond to a valid thread),
  // 					 ULT_NONE(there are no more other threads available to destroy (in response to a call with tid set to ULT_ANY))      

	if(isFirstThread)
		initializeFirstThread();

	if(tid == ULT_ANY && threadCount ==0)
		return ULT_NONE;

	if(tid == ULT_SELF && threadCount == 0)
		return ULT_NONE;

	if(tid == ULT_SELF){
		ThrdCtlBlk *nextThread;
		nextThread = popThread();
		free(runningThread->context);
		runningThread = nextThread;
		setcontext(runningThread->context);
	}

	if(tid == ULT_ANY){
		ThrdCtlBlk *deleteThread = popThread();
		Tid ret = deleteThread->threadID;
		free(deleteThread->context);
		free(deleteThread);
		return ret;
	}

	if(!isValidTid(tid))
		return ULT_INVALID;

	if(threadCount == 0)
		return ULT_NONE;

	ThrdCtlBlk *deleteThread = removeFromReady(tid);
	Tid ret = deleteThread->threadID;
	free(deleteThread->context);
	free(deleteThread);
	// interruptsOn();
	return ret;

	/*
    ****** Be careful. It is dangerous to use memory once it has been freed.
    * In particular, you should not free the stack of the currently
    * running thread while it is still running. (Yet you still need to
    * make sure that that stack eventually gets deallocated.
    * Think about this a bit!) You should convince yourself that
    * your program would work even if you used a debugging malloc library
    * that overwrites a block with dummy data when that block is free()'d.

    ****** Be careful. If you destroy a thread that is holding a lock, deadlock
    * may occur. For this reason, it is usually best to only use DestroyThread
    * to have each thread destroy itself (not other threads.) In fact, the
    * Java library deprecates the equivalent function in their thread library.
    * Note that Java's alternative makes use of the richer exception model
    * that Java has compared to C. We include the more general form here partly
    * as a design exercise and because you may need it for some of your tests.
    */
}

void stub(void (*root)(void *), void *arg) 
{
	// thread starts here 
	Tid ret; 
	root(arg); // call root function 
	ret = ULT_DestroyThread(ULT_SELF);
	assert(ret == ULT_NONE); // we should only get here if we are the last thread. 
	exit(0); // all threads are done, so process should exit

}

