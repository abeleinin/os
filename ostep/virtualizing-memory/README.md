# Virtualizing Memory

- [Address Space](#address-space)
- [Virtual Memory](#virtual-memory)

## Address Space

- **Multiprogramming**: Running multiple processes at a given time, and the OS decides how to switch between them.
- **Time sharing**: Concurrent sharing of compute resource among many jobs by giving each task or user a small slice of processing time.

The address space of a process contains all of the memory state of the running program. Three main components:

- Code (instructions)
- Stack
- Heap

## Virtual Memory

Virtual memory must be implemented such that its invisible to the running program, efficient in terms of time and space, and protects processes from one another. When on eprocess performs a load, a store, or an instruction fetch, it should not be able to acess or affect in any way the memory contents of any other process or the OS itself (nothing *outside* its own address space).