# Concurrency

## Overview

- A thread is similar to a process except, threads *share* the same address space and can access the same data
- Within a process we saved state to a **process control block (PCB)**, to store the state of one or more threads we use a **thread control block (TCB)**
- During a context switch between threads, the address space remains the same (ie theres no need to switch which page table we're using)
- Threads can have **thread-local** storage which is it's own stack within address space
- The order of thread execution depends on the OS scheduler

## Concurrent Vector Addition

- [concurrent_vector_addition.c](./concurrent_vector_addition.c)

## Additional Reading

- [Cooperating sequential processes](https://www.cs.utexas.edu/~EWD/transcriptions/EWD01xx/EWD123.html)

