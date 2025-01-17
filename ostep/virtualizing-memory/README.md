# Virtualizing Memory

- [Address Space](#address-space)
- [Virtual Memory](#virtual-memory)
- [Memory Api](#memory-api)
- [Address Translation](#address-translation)
- [Segmentation](#segmentation)
- [Free-Space Management](#free-space-management)
- [Additional Reading](#additional-reading)

## Address Space

- **Multiprogramming**: Running multiple processes at a given time, and the OS decides how to switch between them.
- **Time sharing**: Concurrent sharing of compute resource among many jobs by giving each task or user a small slice of processing time.

The address space of a process contains all of the memory state of the running program. Three main components:

- Code (instructions)
- Stack
- Heap

## Virtual Memory

Virtual memory must be implemented such that its invisible to the running program, efficient in terms of time and space, and protects processes from one another. When on eprocess performs a load, a store, or an instruction fetch, it should not be able to acess or affect in any way the memory contents of any other process or the OS itself (nothing *outside* its own address space).

## Memory API

- **Stack memory**
    - Allocations/Deallocations: Managed *implicitly* by the compiler for you. This memory is automatically deallocated when a method returns.
    - Lifetime: Limited to the duration of the function.
- **Heap Memory**
    - Allocations/Deallocations: Handled *explicitly* by the programmer. 
    - Allocated with  `malloc()` or `new` in C/C++ 
    - Deallocated with  `free()` or `delete` in C/C++ 

### `malloc()` and `new`

```c
#include <stdlib.h>
/**
 * Allocates `size` bytes of memory
 * 
 * @return void* - pointer to allocated memory
 */
void* malloc(size_t size);

/**
 * Contiguously allocates enough space for `count` objects that are `size` bytes 
 * of memory each. The memory is filled with bytes of value zero.
 * 
 * @return void* - pointer to allocated memory
 */
void* calloc(size_t count, size_t size);

/**
 * Tries to change the size of the allocation pointed to by `ptr` to `size`.
 * If there is not enough room to enlarge the memory allocation pointed to by 
 * `ptr`, realloc() creates a new allocation, copies as much of the old data 
 * pointed to by `ptr` as will fit to the new allocation, frees the old 
 * allocation, and returns a pointer to the allocated memory.
 * 
 * @return void* - pointer to allocated memory
 */
void* realloc(void *ptr, size_t size);

/**
 * Identical to the realloc() function, except that it will free the passed pointer 
 * when the requested memory cannot be allocated. FreeBSD specific API designed to 
 * ease the problems with traditional coding styles for realloc causing memory 
 * leaks in libraries.
 * 
 * @return void* - pointer to allocated memory
 */
void* reallocf(void *ptr, size_t size);

/**
 * Allocates `size` bytes of memory. 
 * The allocated memory is aligned on a page boundary.
 * 
 * @return void* - pointer to allocated memory
 */
void* valloc(size_t size);

/**
 * Allocates `size` bytes of memory with an alignment specified by `alignment`.
 * 
 * @return void* - pointer to allocated memory
 */
void* aligned_alloc(size_t alignment, size_t size);

/**
 * Deallocates the memory allocation pointed to by `ptr`. 
 * If `ptr` is a NULL pointer, no operation is performed.
 * 
 * @return void
 */
void free(void *ptr);
```

```cpp
// C++11
/* throwing  (1) */ void* operator new (std::size_t size);
/* nothrow   (2) */ void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) noexcept;
/* placement (3) */ void* operator new (std::size_t size, void* ptr) noexcept;
```

1. Throwing allocation
    ```cpp
    MyClass* p2 = new MyClass;
    ```
    - Allocates `size` bytes of storage
    - On failure, throw `bad_alloc` exception
    - Use when exceptions are preferred for handling rare allocation failures in high-level applications.
2. nothrow allocation
    ```cpp
    MyClass* p2 = new (std::nothrow) MyClass;
    ```
    - Same as 1., except that on failure it returns a `nullptr` instead of throwing an exception.
    - Use in performance-critical, low-overhead, or exception-free environments where explicit `nullptr` checks are manageable.
3. placement
    ```cpp
    void* buffer = malloc(sizeof(MyClass)); // Pre-allocated memory
    MyClass* obj = new (buffer) MyClass();  // Placement new
    ```
    - Placement new allows you to construct an object at a pre-allocated memory location that you specify.
    - Simply returns `ptr` (no storage is allocated)
    - Even though storage isn’t allocated, the constructor of the object is still called, so that the object is fully initialized.
    - Use when precise memory control is required, such as with pre-allocated buffers, custom allocators, or real-time systems.

Newer programming languages support **automatic memory management** by way of a **garbage collection**.

### Common Errors

- Forgetting to allocate memory
    ```c
    char* src = "hello";
    char* dst;        // unallocated ptr
    strcpy(dst, src); // segfault
    ```
- **Buffer Overflow**: Not allocating enough memory 
    ```c
    char* src = "hello";
    char* dst = (char*) malloca(strlen(src)); // too small
    strcpy(dst, src); // works properly
    ```
    - The code above often runs, but it still is incorrect because the `strcpy` writes 1 bit past the end of the allocated space. 
    - Overflow can be the cause of many security vulnerabilities
- Forgetting to initialize allocated memory
    ```c
    int* arr = (int*) malloc(sizeof(int) * 5);
    printf("%d\n", arr[0]); // arr[0] is never initialized to a value
    ```
- **Memory Leak**: Forgetting to free memory
    ```c
    int main () {
        int* arr = (int*) malloc(sizeof(int) * 5);
        // free(arr); // missing free
        return 0;
    }
    ```
    - Get into the habit of freeing each and every byte you explicity allocate.
    - Garbage-collected languages are still susceptible if you have a reference to some memory, the garbage collector will never free it
    - No memory is leaked once a process exits because the operating system will reclaim *all* the memory of the process (consider bad form)
- **Dangling pointer**: Pointer to deallocated or freed memory.
    ```c
    int* arr = (int*) calloc(5, sizeof(int));
    printf("arr[0]=%d\n", arr[0]);
    free(arr);
    printf("arr[0]=%d\n", arr[0]); // arr is a dangling pointer
    ```
- **Double free**: Freeing memory more than once
    ```c
    int* arr = (int*) calloc(5, sizeof(int));
    free(arr);
    free(arr); // double free
    // malloc: *** error for object 0x600002071240: pointer being freed was not allocated
    ```

## Address Translation

- **Address translation**: Transforms hardware memory accesses (ie, an instruction fetch, load, or store), changing the virtual address provided by the instruction to a physical address where the desired information is actually located.
- Goal: Programs should have their own private memory, where its own code and data resides.
- Disassemble tools: Linux=`objdump`, Mac=`otool`, or Compiler Explorer online
- **Address space**: From the programmers perspective, address space starts at address 0 and grows to 16 KB.

### Dynamic (Hardware-based) Relocation

- Two hardware registers within each CPU: one is called the **base** register, and the other the **bound** (or **limit** register).
- The base registers is used to transform virtual addresses into physical addresses. A bound (or limit) register ensures that such addresses are within the confines of the address space
- **Memory Management Unit (MMU)** On-chip circuitry to support sophisticated memory-management techniques.
- **Program status word** is a register that performs the function of a status register and program counter. This is where we could indicate to the CPU which mode we're in.
- **Process control block (PCB)**
    - A data structure that stores information about a process being managed by an OS
    - Process state, program counter, register values (base/bound), I/O device allocation, priority, open files
- **Internal fragmentation**: Allocated space within a process that's not being used (a fragment). Space between stack and heap.

## Segmentation 

Memory management technique where a process's addres space is divided into multiple, variable-sized segments. Each logical division of memory are assigned base and bound pairs per segment. 

- **Sparse address space**: Address spaces with large amounts unused memory.
- **Segmentaion fault**: Arises from memory access on a segmented machine to an illegal address. The term persists in computing, even on machines with no support for segmentation.

### Explicit Segmentation

Chop up the address space into segments based on the top few bits of the virtual address.

Implementation of explicit segmentation from address space to physical memory ([OSTEP Segmentation](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-segmentation.pdf#page=3)).

```c
// 14-bit virtual address
//
// Segment | Offset
//   13 12 | 11 10 09 08 07 06 05 04 03 02 01 00
//    0  1 |  0  0  0  0  0  1  1  0  1  0  0  0
```

- [explicit_segmentation.c](explicit_segmentation.c)

```c
#include <stdio.h>
#include <signal.h>

#define SEG_MASK 0x3000
#define SEG_SHIFT 12
#define OFFSET_MASK 0xFFF

// Segment sizes:
// Code 2K, Heap 3K, Stack 2K
int Bounds[3] = {2048, 3072, 2048};

// Base (Physical address):
// Code 32K, Heap 34K, Stack 28K
int Base[3] = {32768, 34816, 28672};

int main() {
  // get top 2 bits of 14-bit virtual address
  // virtual address is just 4200 in decimal
  unsigned int virtualAddress = 0b01000001101000;
  int segment = (virtualAddress & SEG_MASK) >> SEG_SHIFT;

  // get offset
  int offset = virtualAddress & OFFSET_MASK;

  printf("segment=%d\n", segment);
  printf("offset=%d\n", offset);

  // hypothetically...
  if (offset >= Bounds[segment]) {
    printf("RaiseException(PROTECTION_FAULT)\n");
    raise(SIGSEGV);
  } else {
    int physicalAddress = Base[segment] + offset;
    printf("register=AccessMemory(PhysicalAddress=%d)\n", physicalAddress);
  }

  return 0;
}
```

### Fine-grained vs. Course-grained Segmentation

- **Course-grained**: Systems with just a frew segments (ie, code, stack, heap)
- **Fine-grained**: Early systems were more flexible and allowed for address spaces to consist of many smaller segments.
    - Many segments require additional hardware support with a **segment table**

### OS Support

- `brk` and `sbrk` system calls change the amount of space allocated for the calling process's data segment
    - `brk` function sets the break value to ends and changes the allocated space accordingly.
    - `sbrk` functions adds `incr` function bytes to the break value and changes the allocated space accordingly.

## Free-Space Management

- [free_space_management.c](./free_space_management.c)

- **External fragmentation**: Free space gets chopped into little pieces of different sizes and is thus fragmented; subsequent requests may fail because there's no single contiguous space.
- **Internal fragmentation**: If an allocator hands out chunks of memory bigger than that requested.

### Splitting and Coalescing 

- **Splitting**: Finds available memory chunk that satisfies the request and splits it into two.
    - Commonly used when requests are smaller than the size of any particular free chunk.
- **Coalescing**: If a newly free chunk has a neighbor that's also free, merge them into a single larger free chunk.

### Tracking allocated regions size

- Spoiler... Allocators store extra information in a **header** block kept above the allocated space.

```c
typedef struct {
    int size;
    int magic;
} header_t;
```

- Headers contain:
    - `size` of allocated region
    - `magic` number for integrity checking
    - sometimes additional pointers to speed up deallocation

When a user calls the `free` function, it might looks something like:

```c
void free(void* ptr) {
    header_t* hptr = (header_t*) ptr - 1;
}
```

When a user requests *N* bytes of memory, the library actually allocates a chunk of *N* + `sizeof(header_t)`.

Now, here is what the **Free List** data structure might look like:

```c
typedef struct __node_t {
    int              size;
    struct __node_t *next;
} node_t;
```

### Managing Free-Space 

> Trying to optimize for both speed and minimizing fragmentation.

1. **Best Fit**: Search the entire list, find the smallest chunk that fits the users request.
2. **Worst Fit**: Search the entire list, find the largest chuck and return the requested amount.
3. **First Fit**: Finds the first chuck that's big enough and returns the requested amount.
4. **Next Fit**: Same as first fit, but uses an extra pointer to the location you we're previous looking.

### Other Approaches

- **Segregated lists**: If a particular process has one (or a few) popular-sized requests, keep a separate list just to manage object of that size; all other requests are forwarded to a more general memory allocator.
- **Slab allocator**: When a given cache runs low on free space it requests some **slabs** of memory from a more general memory allocator. Slab allocator avois frequent initialization and destruction sycles by keeping freed objects in a particular list in their initialized state thus lowering overhead.
- **Buddy allocation**: Free memory is conceptually thought of as one big space of size $2^N$. Uses recursive search that divides free space by two until a block big enough to accomodate the request is found. When freeing data, the allocator checks if their "buddy" (or neighboring block) is free; if so, it coalesces the two blocks.

## Additional Reading

- [Intel 64 and IA-32 Architectures](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [ARM Architecture Reference Manual](https://www.cs.utexas.edu/~simon/378/resources/ARMv7-AR_TRM.pdf)
- [Dynamic Storage Allocation: A Survey and Critical Review](https://www.cs.hmc.edu/~oneill/gc-library/Wilson-Alloc-Survey-1995.pdf)
- [A Scalable Concurrent malloc(3) Implementation for FreeBSD](https://hydra.azilian.net/Papers/jemalloc.pdf)
- [Understanding glibc malloc](https://sploitfun.wordpress.com/2015/02/10/understanding-glibc-malloc/)
