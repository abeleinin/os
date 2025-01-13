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
