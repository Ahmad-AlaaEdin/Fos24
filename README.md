# FOS (Fictional Operating System) – Ain Shams University CSW355 Project

**FOS** is an educational, Unix-like operating system designed as a core project for the Operating Systems course (CSW355) at Ain Shams University. It is a forked and modularly refactored version of the renowned MIT Operating Systems Lab (6.828).

This project aims to provide hands-on experience in implementing key OS components such as memory management, process synchronization, scheduling, and more.

---

## 🧠 Project Structure

The project is divided into multiple milestones:

### 📍 Milestone 1: Preprocessing & Core Utilities
- Command prompt with argument parsing
- Basic system calls
- **Dynamic Memory Allocator**
  - `alloc_block_FF()`
  - `free_block()`
  - `realloc_block_FF()`

- **Sleep Locks** (non-busy synchronization)
  - `sleep()`, `wakeUpOne()`, `wakeUpAll()`
  - `acquire()`, `release()`

---

### 📍 Milestone 2: Memory Management
- **Kernel Heap**
  - `kmalloc()`, `kfree()`
  - `kheap_virtual_address()`, `kheap_physical_address()`
  - `sbrk()`

- **User Heap**
  - `malloc()`, `free()`, `realloc()`
  - Backed by kernel system calls
  - Managed with a user-space dynamic allocator

- **Fault Handler I**
  - Page placement logic (placement on fault)
  - Validity and boundary checks
  - Lazy allocation support

- **Shared Memory**
  - `smalloc()`, `sget()` for interprocess communication

---

### 📍 Milestone 3: CPU Features
- **Page Fault Handler II**
  - Implemented Nth-Chance Clock replacement algorithm
  - Handles normal and modified pages
  - Integration with page working set management

- **User-Level Semaphores**
  - `create_semaphore()`, `get_semaphore()`
  - `wait_semaphore()`, `signal_semaphore()`
  - FIFO queue-based waiting and synchronization

- **Priority Round-Robin Scheduler**
  - Multi-level ready queues based on priority
  - Preemptive scheduling with starvation prevention
  - Promotion via starvation threshold
  - System calls to change priority and initialize scheduler

---

## 🧑‍💻 My Role

I contributed to the **system-level implementation** of key kernel functionalities:

- ✅ **Dynamic Memory Allocator**  
  - First-Fit allocator (`alloc_block_FF`, `free_block`, `realloc_block_FF`)

- ✅ **Sleep Locks & Synchronization Primitives**  
  - Sleep-based locks (`sleep`, `wakeUpOne`, `wakeUpAll`, `acquire`, `release`)

- ✅ **Kernel Heap Management**  
  - `kmalloc()`, `kfree()` and address conversion utilities

- ✅ **Page Fault Handler (Fault Handler II)**  
  - Implemented Nth-Chance Clock algorithm for page replacement

- ✅ **User Heap Management**  
  - System-level logic for user dynamic allocation and deallocation

- ✅ **Shared Memory APIs**  
  - Enabling processes to communicate via shared virtual memory

- ✅ **Semaphores Implementation**  
  - Implemented full lifecycle (create, wait, signal) with lock protection

- ✅ **Priority RR Scheduler**  
  - Designed and integrated a multilevel, starvation-aware scheduler

---

## 🚀 Getting Started

### Prerequisites
- x86 Emulator (QEMU / Bochs)
- GCC & GNU Make


### Running FOS
```bash
# Example FOS prompt commands
FOS> run tst_dynalloc_allocff
FOS> run tst_kheap_kmalloc
FOS> schedPRIRR 5 10 1000
FOS> run fib 20 3
FOS> runall
```

## 🛠️ Project Tools & Strategies

- **Development Language:** C  
- **Architecture:** x86 protected mode  
- **Paging:** Enabled (with page file and working set)  
- **Memory Management:** First-fit with coalescing  
- **Synchronization:** Locks & Semaphores  
- **Scheduling:** Round-robin, Priority RR  
- **Testing:** Built-in scripts and test programs  

---

## 📎 Notes

- All shared data structures are synchronized via spinlocks/sleep locks.

