#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* We want the extra information from these definitions */
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <ucontext.h>

#define SOL 99

unsigned int probeUCStack(char *string);

int main(int argc, char **argv)
{

  ucontext_t mycontext;
  int err;

  // NGREG - Number of general purpose registers 
  // (19 for 32 bit and 23 for 64 bit)
  
  // __WORDSIZE - 64 bit or 32 bit
  // DEfined at https://gcc.gnu.org/bugzilla/attachment.cgi?id=21104

  printf("\n\nWORDSIZE %d\n", __WORDSIZE);
  assert(__WORDSIZE == 32); // Do this project on a 32-bit x86 linux machine
  printf("NGREG %d\n", NGREG);
  assert(NGREG == 19);  // Do this project on a 32-bit x86 linux machine

  err = getcontext(&mycontext);
  assert(!err);

  printf("After saving context mycontext.uc_stack.ss_sp %x\n", (unsigned int)mycontext.uc_stack.ss_sp);

  printf("A ucontext_t is %d bytes\n", sizeof(mycontext));
  // assert(0); // TBD: Fill in ucontext size above. Hint: use sizeof().

  unsigned int anotherSample = probeUCStack("Dummy argument.");

  /* 
   * Now, look inside of the ucontext you just saved.
   *
   * Warning: The automated grading scrips won't check all of these.
   * Sanity check them yourself. Think about what they mean and what
   * they must be!
   *
   * For example, are both argc and err on the stack? 
   */
  /*
   * First, think about program counters (called eip in x86)
   */
  printf("The memory address of the function main() is 0x%x\n", (unsigned int)&main);
  printf("The memory address of the program counter (EIP) saved in mycontext is 0x%x\n", (unsigned int)mycontext.uc_mcontext.gregs[REG_EIP]);

  /*
   * Now, think about stacks. 
   *
   * Be able to answer the following questions: Does the stack grow up or down? - DOWN
   
   * Does ucontext_t.uc_stack point to the current stack pointer? Top of the
   * stack? Bottom of the stack? None of the above? - BOTTOM OF THE STACK
   
   * What are the stack related 
   * things in mycontext.uc_mcontext.gregs[]? - 

   * What's the difference between
   * REG_UESP and REG_ESP in the gregs array? - REG_UESP IS USER ESP WHILE REG_ESP IS KERNEL ESP
   *
   * Hint: It might help to run the program using gdb or xxx (a graphical
   * front end to gdb) and put a breakpoint at entry to main and/or the 
   * call to getcontext().  Then use "info registers" to see the values 
   * of the registers and use "next"/"step" to advance to the next instruction.
   * You can also use "print mycontext" to see the values stored in that struct.
   *
   * Another experiment you might try is changing the various candidates for the
   * stack pointer to nonsensical values and (probably using a debugger) see
   * which one actually gets restored to the processor.
   *
   * Don't move on to the next part of the lab until you know how to change
   * the stack in a context when you manipulate a context to create a new thread.
   */
  printf("\n\nThe memory address of the local variable err is 0x%x\n", (unsigned int) &err);
  printf("The memory address of the argument argc is 0x%x\n", (unsigned int) &argc);
  printf("The value of ucontext_t.uc_stack is 0x%x\n", (unsigned int)mycontext.uc_stack.ss_sp);
  printf("The value of anotherSample is 0x%x\n", anotherSample);
  printf("The stack pointer stored as one of the registers (ESP) in uc_mcontext is 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_ESP]);
  printf("The stack pointer stored as another one of the `registers` (UESP) in uc_mcontext is 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_UESP]);

  printf("\n\nThe number of bytes pushed onto the stack between argc and err was 0x%x\n", (unsigned int)(&argc - &err));
  /* Which is the right one to use? */
  printf("The number of bytes pushed onto the stack between err and when the stack was saved to mycontext was 0x%x\n\n", 
         (unsigned int)((unsigned int)&err - (unsigned int)mycontext.uc_mcontext.gregs[REG_ESP]));

  printf("\n\nREG_GS 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_GS]);
  printf("REG_FS 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_FS]);
  printf("REG_ES 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_ES]);
  printf("REG_DS 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_DS]);
  printf("REG_EDI 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_EDI]);
  printf("REG_ESI 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_ESI]);
  printf("REG_EBP 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_EBP]);
  printf("REG_ESP 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_ESP]);
  printf("REG_EBX 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_EBX]);
  printf("REG_EDX 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_EDX]);
  printf("REG_ECX 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_ECX]);
  printf("REG_EAX 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_EAX]);
  printf("REG_TRAPNO 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_TRAPNO]);
  printf("REG_ERR 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_ERR]);
  printf("REG_EIP 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_EIP]);
  printf("REG_CS 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_CS]);
  printf("REG_EFL 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_EFL]);
  printf("REG_UESP 0x%x\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_UESP]);
  printf("REG_SS 0x%x\n\n\n", (unsigned int)(unsigned int)mycontext.uc_mcontext.gregs[REG_SS]);

  return 0;  
}


/*
 * This function should call getcontext and
 * return the value of the uc_stack.ss_sp.
 * 
 * Is this the current stack pointer? The
 * top of the stack? The bottom of the stack?
 * One thing to do is to compare it to the
 * uc_stack.ss_sp saved in main().
 */
unsigned int 
probeUCStack(char *str)
{
  ucontext_t mycontext;

  int ret;
  ret = getcontext(&mycontext);
  assert(!ret);
  printf("Value of uc_stack.ss_sp %x\n", (unsigned int)mycontext.uc_stack.ss_sp);
  return (unsigned int) mycontext.uc_stack.ss_sp;
}
