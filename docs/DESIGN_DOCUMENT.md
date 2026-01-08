# OS Memory Simulator - Design Document

## Overview

This document describes the architecture and design of the OS Memory Simulator, an educational tool demonstrating basic memory management techniques including contiguous allocation strategies, buddy allocation, cache simulation, and virtual memory concepts.

## Table of ContentsS

- [System Architecture](#system-architecture)
- [Memory Layout and Assumptions](#memory-layout-and-assumptions)
- [Physical Memory Management](#physical-memory-management)
- [Allocation Strategy Implementations](#allocation-strategy-implementations)
- [Buddy System Design](#buddy-system-design)
- [Cache Hierarchy](#cache-hierarchy)
- [Command Explanations](#commands)
- [Performance Characteristics](#performance-expectations)
- [Limitations](#limitations)
- [Test Artifacts](#test-artifacts)

---

## System Architecture

### Component Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    OS Memory Simulator                      │
├─────────────────────────────────────────────────────────────┤
│                 Main Interface (main.cpp)                   │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  User Commands: malloc, free, dump, stats, etc.     │    │
│  └─────────────────────────────────────────────────────┘    │
│                            │                                │
│                ┌───────────┼───────────┐                    │
│                │           │           │                    │
│                ▼           ▼           ▼                    │
│          ┌─────────┐ ┌─────────┐ ┌─────────┐                |
│          │Physical │ │ Buddy   │ │Cache    │                │
│          │Memory   │ │ Alloc   │ │System   │                │
│          └─────────┘ └─────────┘ └─────────┘                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Design Principles

1. **Modularity**: Each component (PhysicalMemory, Buddy, Cache) is self-contained
2. **Educational Focus**: Clear, understandable implementations over optimized performance
3. **Separation of Concerns**: Memory allocation, caching, and address translation are handled by distinct components
4. **Interactive Interface**: User-friendly command-line interface for experimentation

---

## Memory Layout and Assumptions

### Physical Memory Model

**Memory Size**: 256 bytes (configurable)
**Address Space**: Linear, byte-addressable
**Granularity**: 1 byte

If we have to allocate 15 bytes memory space

```
┌─────────────────────────────────────┐
│          Physical Memory            │
│          (256 bytes)                │
├─────────────────────────────────────┤
│ 0x00                                │
│ ┌─────────────────────────────────┐ │
│ │ Free Block 1                    │ │  ← First-fit allocates here
│ │ Size: 20 bytes                  │ │
│ └─────────────────────────────────┘ │
│ 0x20                                │
│ ┌─────────────────────────────────┐ │
│ │ Free Block 2                    │ │  ← Best-fit allocates here
│ │ Size: 15 bytes                  │ │
│ └─────────────────────────────────┘ │
│ 0x35                                │
│ ┌─────────────────────────────────┐ │
│ │ Free Block 3                    │ │  ← Worst-fit allocates here
│ │ Size: 220 bytes                 │ │
│ └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

### Memory Allocation Strategies

| Strategy  | Time Complexity | Space Efficiency | Fragmentation |
| --------- | --------------- | ---------------- | ------------- |
| First-fit | O(1)            | Moderate         | Low           |
| Best-fit  | O(n)            | High             | Very Low      |
| Worst-fit | O(n)            | Low              | Medium        |

### Assumptions

1. **Contiguous Allocation**: Physical memory is treated as a single contiguous block
2. **No Virtual Memory**: Direct physical memory access (simplified model)
3. **Fixed Size**: Physical memory size is fixed at startup
4. **No Protection**: No memory protection mechanisms implemented
5. **Single Thread**: No concurrent access scenarios

---

## Physical Memory Management

### Data Structures

#### Block Structure

```cpp
struct Block {
    int start;        // Starting address in memory
    int size;         // Size of the block in bytes
    bool free;        // true if block is available, false if allocated
    Block* next;      // Pointer to next block in list
    Block* prev;      // Pointer to previous block in list
};
```

#### Memory Management

- **Doubly Linked List**: Maintains order of memory blocks
- **Header Block**: Represents entire memory space
- **Coalescing**: Adjacent free blocks are merged automatically

### Memory Block Lifecycle

```
Initial State:
┌─────────────────────────────────────┐
│ Header: 0x00-0xFF, FREE, 256B       │
└─────────────────────────────────────┘

After malloc 20 first:
┌───────┬─────────────────────────────┐
│ FREE  │ ALLOCATED: 0x00-0x20, 20B   │
│ 246B  │                             │
└───────┴─────────────────────────────┘

After malloc 15 best:
┌───────┬─────────────┬─────────────────┐
│ FREE  │ ALLOCATED   │ FREE            │
│ 231B  │ 0x00-0x35   │ 0x35-0xFF       │
│       │ 35B         │ 231B            │
└───────┴─────────────┴─────────────────┘
```

### Key Operations

#### allocateFirstFit(int size)

1. Traverse linked list from beginning
2. Find first block where `free == true` and `size >= requested_size`
3. If exact match: Mark as allocated
4. If larger: Split block, allocate first part
5. Return starting address or -1 if no suitable block found

#### allocateBestFit(int size)

1. Traverse entire linked list
2. Find FREE block with minimum size that fits requested allocation
3. Split block if necessary
4. Return starting address or -1 if no suitable block found

#### allocateWorstFit(int size)

1. Traverse entire linked list
2. Find FREE block with maximum size
3. Split block if necessary
4. Return starting address or -1 if no suitable block found

---

## Allocation Strategy Implementations

### First-Fit Algorithm

**Time Complexity**: O(1) average case, O(n) worst case
**Space Complexity**: O(1)

**Advantages**:

- Fast allocation in typical scenarios
- Simple implementation
- Low overhead

**Disadvantages**:

- Can lead to external fragmentation
- May allocate in less optimal locations

### Best-Fit Algorithm

**Time Complexity**: O(n)
**Space Complexity**: O(1)

**Advantages**:

- Minimizes wasted space
- Reduces external fragmentation
- Good space utilization

**Disadvantages**:

- Slower allocation (must scan all blocks)
- May create many small unusable fragments

### Worst-Fit Algorithm

**Time Complexity**: O(n)
**Space Complexity**: O(1)

**Advantages**:

- Reduces fragmentation in some scenarios
- Preserves large blocks for future allocations

**Disadvantages**:

- Slowest allocation strategy
- May waste space by using large blocks for small requests

---

## Buddy System Design

### Overview

The Buddy System is an alternative memory allocation algorithm that:

- Allocates memory in blocks that are powers of 2
- Splits larger blocks in half when smaller blocks are needed
- Merges adjacent buddy blocks when they become free
- Provides fast allocation and deallocation with minimal fragmentation

### Buddy Block Structure

```cpp
struct Block {
    int ord;           // Order of the block
    Block* prev;       // Previous block in free list
    Block* next;       // Next block in free list
};
```

### Buddy System Architecture

```
Initial State (512-byte pool):
┌─────────────────────────────────────────────┐
│ Order 9 Block (512 bytes)                   │
│ Free List: [0x000-0x1FF]                    │
└─────────────────────────────────────────────┘

After allocate(32):
┌───────────────┬────────────────────────────┐
│ Order 5       │ Order 9 Block              │
│ 32 bytes      │ 256 bytes                  │
│ Free List:    │ Free List:                 │
│ [0x000-0x01F] │ [0x020-0x1FF]              │
└───────────────┴────────────────────────────┘
```

### Buddy System Operations

#### Allocation Process

1. **Size Rounding**: Round requested size up to nearest power of 2
2. **Order Finding**: Determine required order: `order = ceil(log2(size))`
3. **Block Search**: Find available block at required order or higher
4. **Block Splitting**: Split larger blocks recursively until exact size achieved
5. **Block Marking**: Mark allocated block as used

#### Deallocation Process

1. **Block Location**: Find buddy block address
2. **Buddy Check**: Verify if buddy block is free
3. **Coalescing**: Merge with buddy if free
4. **Recursive Merge**: Continue merging up the order hierarchy
5. **Block Insertion**: Add merged block to appropriate free list

### Buddy System Characteristics

| Property           | Value                 |
| ------------------ | --------------------- |
| Initial Size       | 512 bytes             |
| Maximum Order      | 9                     |
| Minimum Block Size | 1 byte                |
| Splitting Factor   | 2x                    |
| Coalescing Rules   | Adjacent buddies only |

### Address Calculations

**Block Size**: `size = 2^order`
**Buddy Address**: For block at address `A` with order `k`:

- `buddy_address = A XOR (1 << k)`

**Example**:

- Block at 0x000, order 5 (32 bytes)
- Buddy address = 0x000 XOR 0x020 = 0x020

---

## Cache Hierarchy

### Cache Configuration

#### L1 Cache

- **Size**: 4 KB
- **Block Size**: 4 bytes
- **Associativity**: 1 (Direct-mapped)
- **Sets**: 1024

#### L2 Cache

- **Size**: 16 KB
- **Block Size**: 4 bytes
- **Associativity**: 1 (Direct-mapped)
- **Sets**: 4096

### Cache Line Structure

```cpp
struct CacheLine {
    bool valid;        // Valid bit
    int tag;           // Tag bits
    long long fifoTime; // FIFO replacement timestamp
};
```

### Cache Organization

```
┌─────────────────────────────────────────────────────────┐
│                    L1 Cache (4KB)                       │
├─────────────────────────────────────────────────────────┤
│ Set 0: [Valid:0, Tag:0x000, FIFO:0]                     │
│ Set 1: [Valid:0, Tag:0x000, FIFO:1]                     │
│ ...                                                     │
│ Set 1023: [Valid:0, Tag:0x000, FIFO:1023]               │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│                    L2 Cache (16KB)                      │
├─────────────────────────────────────────────────────────┤
│ Set 0: [Valid:0, Tag:0x000, FIFO:0]                     │
│ Set 1: [Valid:0, Tag:0x000, FIFO:1]                     │
│ ...                                                     │
│ Set 4095: [Valid:0, Tag:0x000, FIFO:4095]               │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│                 Main Memory (256B)                      │
├─────────────────────────────────────────────────────────┤
│ 0x000-0x0FF: Physical Memory Space                      │
│ 0x100-0x1FF: Extended Space                             │
└─────────────────────────────────────────────────────────┘
```

### Address Translation

**Address Format** (for 16-bit addresses):

```
┌─────────┬─────────┬──────────┐
│ Tag (7) │ Index(5)│ Offset(4)│
└─────────┴─────────┴──────────┘
```

### Cache Access Flow

```
1. CPU generates memory address
   │
   ▼
2. Extract index and offset
   │
   ▼
3. Check L1 cache at computed index
   │
   ├─HIT─┐
   │     │
   ▼     ▼
   │  Return data
   │
   └─MISS─┐
          │
          ▼
4. Check L2 cache
   │
   ├─HIT─┐
   │     │
   ▼     ▼
   │  Update L1, return data
   │
   └─MISS─┐
          │
          ▼
5. Fetch from memory
   │
   ▼
6. Update both caches, return data
```

### Replacement Policy: FIFO

**Implementation**:

- Each cache line maintains a timestamp
- On cache miss, evict the line with oldest timestamp
- Update timestamp on line replacement

**Advantages**:

- Simple implementation
- Predictable behavior
- Good for sequential access patterns

**Disadvantages**:

- Can evict frequently used lines
- Not adaptive to access patterns

---

## Commands

```bash
malloc <size> <strategy>
```

- **What happens**: Allocates `<size>` bytes using specified strategy
- **First-fit**: Finds first available block (fastest, O(1))
- **Best-fit**: Finds smallest block that fits (efficient memory usage, O(n))
- **Worst-fit**: Finds largest available block (reduces fragmentation, O(n))

```bash
free <address>
```

- **What happens**: Frees memory block starting at `<address>`
- **First/Best/Worst-fit**: Marks block as free, merges with adjacent free blocks
- **Buddy**: Deallocates buddy block and attempts coalescing with buddy

```bash
dump
```

- **What happens**: Shows current memory layout
- **Output**: List of memory blocks with start address, size, and FREE/USED status

```bash
stats
```

- **What happens**: Shows allocation performance metrics
- **Output**: Success/failure rates, memory utilization, largest free block

```bash
CacheAccess <address>
```

- **What happens**: Simulates memory access at `<address>`
- **L1 Cache**: 4KB, 4-byte blocks, direct-mapped (4 sets)
- **L2 Cache**: 16KB, 4-byte blocks, direct-mapped (16 sets)
- **Hit**: Data found in cache (fast access)
- **Miss**: Data not in cache (slower access from lower level)

```bash
Cachestats
```

- **What happens**: Shows cache performance statistics
- **Output**: Hit rates for L1 and L2 cache, total accesses

```bash
BuddyAlloc <size>
```

- **What happens**: Allocates power-of-2 sized block
- **Splitting**: Large blocks split in half until size fits
- **Merging**: When buddy blocks are freed, they merge back together

```bash
BuddyFree <address>
```

- **What happens**: Frees buddy-allocated memory block at the specified address
- **Coalescing**: Checks if buddy block is free and merges if possible
- **Recursive Merging**: Continues merging up the order hierarchy
- **Output**: Confirms memory block has been freed

## Performance Expectations

#### Memory Allocation

- **First-fit**: Fastest allocation (O(1)), moderate fragmentation
- **Best-fit**: Slower allocation (O(n)), least memory waste
- **Worst-fit**: Slowest allocation (O(n)), most fragmentation resistance

#### Cache Performance

- **Sequential access**: High spatial locality → >80% hit rate expected
- **Random access**: Low locality → <20% hit rate expected
- **Loop patterns**: High temporal locality → >90% hit rate expected

#### Buddy Allocator

- **Fast allocation**: O(log n) where n is largest block size
- **Low fragmentation**: Power-of-2 alignment ensures efficient reuse
- **Coalescing**: Adjacent buddy blocks merge automatically

## Limitations

#### Memory Management

1. **Fixed Memory Size**: Cannot dynamically resize memory pool
   - Physical memory fixed at 256 bytes
   - Buddy system limited to 512 bytes
   - No runtime memory reconfiguration
2. **No Memory Protection**: No bounds checking or access control
   - Users can free any address
   - No segmentation fault simulation
3. **Limited Cache Policies**: Only FIFO replacement implemented
4. **Fixed Cache Sizes**: Cannot modify cache configuration at runtime
   - L1: 4KB, direct-mapped, 4-byte lines
   - L2: 16KB, direct-mapped, 4-byte lines
   - No dynamic reconfiguration
5. **Limited Scalability**: Designed for small memory sizes
   - Optimal for educational demonstrations
   - Easy visualization and debugging
   - Limited to proof-of-concept scale

## Test Artifacts

Format: `<TYPE> <command>` where TYPE is WORKLOAD, CACHE, or VIRTUAL

**WORKLOAD tests** - Memory allocation patterns:

- First-fit, Best-fit, Worst-fit allocation strategies
- Memory fragmentation and coalescing scenarios
- Buddy allocator operations (alloc/free)
- Memory dump and statistics validation

**CACHE tests** - Cache performance patterns:

- Sequential access (high spatial locality, >80% hit rate expected)
- Strided access (medium locality, <25% hit rate expected)
- Loop patterns (high temporal locality, >90% hit rate expected)
- Cache statistics validation

**VIRTUAL tests** - Address space management:

- Virtual address allocation and deallocation
- Memory layout verification with dump commands
- Address translation validation
- Memory statistics across operations

**How Test Comparison Works:**

The test system uses **output pattern matching** rather than exact text comparison:

1. **ACTUAL OUTPUT**: Captured from simulator commands (e.g., "Allocated 32 bytes at address 0")
2. **EXPECTED PATTERNS**: Defined in test runner code using regex patterns
3. **COMPARISON**:
   - For `malloc` commands: Checks for "Allocated" OR "Allocation failed"
   - For `free` commands: Checks for "Freed" OR "freed"
   - For `CacheAccess` commands: Checks no "error" in output
   - For `BuddyAlloc` commands: Checks for "buddy allocated" OR "Buddy allocation failed"
   - For status commands (`dump`, `stats`): Just checks they complete without crashing

This approach handles variations in output format while ensuring core functionality works correctly.

### Running Tests

#### Manual Testing

```bash
make
./out

# Example test sequence (in the simulator):
mem> malloc 32 first
Allocated 32 bytes at address 0
mem> malloc 64 best
Allocated 64 bytes at address 32
mem> dump
# Shows memory layout
mem> stats
# Shows allocation statistics
mem> CacheAccess 0
# Accesses address 0 through cache hierarchy
mem> Cachestats
# Shows cache hit/miss statistics
mem> BuddyAlloc 64
# Uses buddy allocator
mem> exit
```

#### Automated Testing

The C++ test runner (`tests/test_runner.cpp`) automates all tests:

**Windows:**

```powershell
g++ -std=c++17 -Iinclude tests/test_runner.cpp src/allocator/PhysicalMemory.cpp src/buddyAllocator/Buddy.cpp src/cache/Cache.cpp src/cache/Multilevel.cpp -o test_runner
.\test_runner
```

**Linux/Ubuntu:**

```bash
g++ -std=c++17 -Iinclude tests/test_runner.cpp src/allocator/PhysicalMemory.cpp src/buddyAllocator/Buddy.cpp src/cache/Cache.cpp src/cache/Multilevel.cpp -o test_runner
./test_runner
```
