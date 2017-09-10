# User Level Thread Package

Pre-emptive user level thread library

Built a preemptive user-level thread library in C that has functions to create threads, destroy threads, and yield threads to control the scheduling underneath. Used context switching for saving and restoring the the thread state for thread preemption (manipulated the fields in the saved ucontext_t directly without using makecontext). Created a program that creates and destroys threads using this library and uses POSIX signals that cause the scheduler to switch from one thread to another.

