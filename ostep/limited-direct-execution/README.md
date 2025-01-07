# Limited Direct Execution

- [Restricted Operations](#restricted-operations)
- [Switching Between Processes](#switching-between-processes)
- [Saving and Restoring Context](#saving-and-restoring-context)
- [Additional Reading](#additional-reading)

## Restricted Operations

### User Mode and Kernel Mode

- User Mode: A restricted state in the operating system where access to hardware and system resources is limited.
- Kernel Mode: A privileged state allowing unrestricted access to system resources and hardware.

### System calls

- A system call is a mechanism for user programs to request services from the operating system.
- Key steps:
    1. A program executes a special trap instruction to invoke a system call.
    2. The OS processes the request and then returns control via a return-from-trap instruction, which also reduces privilege back to user mode.
- **Trap Table:** A data structure set up by the kernel during boot that defines locations of trap handlers.
- **System Call Numbers:**
    - Each system call is assigned a unique number.
    - User code specifies the desired system call by placing its number in a register or a specific stack location.

### Limited direct execution (LDE) protocol

1. Boot time: The kernel initializes the trap table, and the CPU remembers its location for subsequent use.
2. Process initialization: The kernel sets up necessary resources (e.g., process list entry, memory allocation) and uses a return-from-trap instruction to start process execution.
3. System call execution: A process traps into the OS for services, and the OS handles the request before returning control to the process.

## Switching Between Processes

Since a running process occupies the CPU, the OS must regain control to manage processes effectively. This is achieved in two ways:

1. Cooperative Approach:
    - The OS waits for the process to issue a `yield()` system call or perform an illegal operation.
    - The `yield()` system call transfers control back to the OS, allowing it to run other processes.
2. Non-Cooperative Approach:
    - A timer device is programmed to raise an interrupt periodically.
    - When the interrupt occurs, the running process is halted, and the OS executes a pre-configured interrupt handler to regain control.

## Saving and Restoring Context

A **context switch** is the process of saving the state of a currently running process and restoring the state of another process to allow it to run. This is a critical feature of multitasking operating systems, enabling them to run multiple processes seemingly simultaneously.

- State to Save:
    - User Registers: Saved automatically by the hardware during an interrupt or trap.
    - Kernel Registers: Explicitly saved by the operating system when switching processes.
    - Process-Specific Information: Includes the program counter (PC), stack pointer (SP), and other CPU state variables.
    - Memory and I/O State: Typically left unchanged during a context switch.
- When a Context Switch Happens:
    - A timer interrupt signals the OS to preempt the current process.
    - A process voluntarily yields the CPU (ie via a `yield()` system call).
    - The process blocks while waiting for an I/O operation to complete.

### Xv6 Context Switch Code 

```asm
# void swtch(struct context *old, struct context *new);
#
# Save current register context in old
# and then load register context from new.
.globl swtch
swtch:
  # Save old registers
  movl 4(%esp), %eax # put old ptr into eax
  popl 0(%eax) # save the old IP
  movl %esp, 4(%eax) # and stack
  movl %ebx, 8(%eax) # and other registers
  movl %ecx, 12(%eax)
  movl %edx, 16(%eax)
  movl %esi, 20(%eax)
  movl %edi, 24(%eax)
  movl %ebp, 28(%eax)

  # Load new registers
  movl 4(%esp), %eax # put new ptr into eax
  movl 28(%eax), %ebp # restore other registers
  movl 24(%eax), %edi
  movl 20(%eax), %esi
  movl 16(%eax), %edx
  movl 12(%eax), %ecx
  movl 8(%eax), %ebx
  movl 4(%eax), %esp # stack is switched here
  pushl 0(%eax) # return addr put in place
  ret # finally return into new ctxt
```

### Context Saving

- **Timer Interrupt:** Automatically saves user registers to the kernel stack of the interrupted process.
- **Explicit Save:** The operating system explicitly saves kernel registers to a process control block (PCB) or equivalent data structure.
- **Context Restoring:** When switching from process A to process B, the saved context of process A is stored, and the context of process B is loaded to resume execution.

## Questions

1. How does the hardware know the location of the trap tables?
    - The kernel must execute an instruction to inform the hardware of the trap table locations.

2. What could go wrong if a malicious program could install its own trap table?
    - A program with control over the trap table could hijack system calls, execute unauthorized operations, and compromise the entire system's security.

## Additional reading

- [lmbench: Portable tools for performance analysis](https://www.usenix.org/legacy/publications/library/proceedings/sd96/full_papers/mcvoy.pdf)
- [Why Aren't Operating Systems Getting Faster as Fast as Hardware](https://web.stanford.edu/~ouster/cgi-bin/papers/osfaster.pdf)
