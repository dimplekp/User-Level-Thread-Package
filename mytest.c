#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "basicThreadTests.h"
#include "interrupt.h"
#include "ULT.h"
/*
 *  Using posix mutex for some tests of phases 1 and 2
 */
#include <pthread.h> 

static const int verbose = 0;
static const int vverbose = 0;

static void helloo(char *msg);

int main(int argc, char **argv)
{
	Tid ret;
  Tid ret2;
  
  printf("Starting tests...\n");

	ret = ULT_CreateThread((void (*)(void *))helloo, "World");
  assert(ULT_isOKRet(ret));
}

static void helloo(char *msg)
{
  Tid ret;

  printf("Made it to helloo() in called thread\n");
  printf("Message: %s\n", msg);
}
