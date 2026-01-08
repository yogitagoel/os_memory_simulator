# OS Memory Simulator

A small C++ simulator demonstrating basic memory management techniques:

- Contiguous allocation strategies: First-fit, Best-fit, Worst-fit (Physical memory).
- A simple Buddy allocator.
- A simple cache simulator and a two-level (multilevel) cache wrapper.
- Interactive CLI to allocate/free memory, inspect memory layout and statistics, exercise cache, and use the buddy allocator.

This project is intended as an educational tool for understanding allocation strategies and simple cache behavior.

## Project structure

- include/
  - PhysicalMemory.h — interfaces for contiguous memory allocator (doubly linked list of blocks).
  - Cache.h — single-level cache simulator.
  - MultilevelCache.h — wrapper for multi-level cache usage.
  - Buddy.h — buddy allocator interface.
- src/
  - allocator/ — implementation for first/best/worst fit and related stats.
  - buddyAllocator/ — buddy allocator implementation.
  - cache/ — cache implementation.
  - main.cpp — interactive command-line program.
- tests/
  - test_cases.txt — Combined test cases for all operations (workload, cache, virtual)
  - expected_outputs.txt — Expected outputs for all test cases.
  - test_runner.cpp — C++ automated test runner
- docs/
  - DESIGN_DOCUMENT.md — Comprehensive design and architecture documentation
- Makefile — simple targets for running and cleaning.

## Requirements

- g++ (GNU C++ compiler) — C++17 or later recommended.
- Unix-like shell for Makefile targets (the Makefile uses shell/globs).

## Build & run

### Windows (PowerShell)

#### Build the Simulator

```powershell
g++ -std=c++17 -Iinclude src/main.cpp src/allocator/PhysicalMemory.cpp src/buddyAllocator/Buddy.cpp src/cache/Cache.cpp src/cache/Multilevel.cpp -o out
```

#### Run Simulator Manually

```powershell
.\out
```

#### Build Test Runner

```powershell
g++ -std=c++17 -Iinclude tests/test_runner.cpp src/allocator/PhysicalMemory.cpp src/buddyAllocator/Buddy.cpp src/cache/Cache.cpp src/cache/Multilevel.cpp -o test_runner
```

#### Run Automated Tests

```powershell
.\test_runner
```

### Linux/Ubuntu

#### Quick (Makefile):

```bash
make run     # This compiles and runs the simulator
make clean   # Removes the compiled binary (out)
```

#### Manual compile:

```bash
g++ -std=c++17 -Iinclude src/*/*/*.cpp src/*/*.cpp -o out
./out
```

#### Build Test Runner:

```bash
g++ -std=c++17 -Iinclude tests/test_runner.cpp src/allocator/PhysicalMemory.cpp src/buddyAllocator/Buddy.cpp src/cache/Cache.cpp src/cache/Multilevel.cpp -o test_runner

# Run all tests
./test_runner
```

## Interactive CLI — available commands

Start the program (`./out`), then use the `mem>` prompt.

- malloc <size> <first|best|worst>

  - Allocate a contiguous block of `<size>` bytes with the chosen strategy.
  - Example: `malloc 32 first`
  - On success prints: `Allocated <size> bytes at address <start>`

- free <address>

  - Free a previously allocated block that begins at address `<address>`.
  - Example: `free 0`

- dump

  - Print the current physical memory layout (blocks with ranges, FREE/USED and size).

- stats

  - Print allocation statistics: free/allocated memory, largest free block, utilization, allocation success/failure rates, etc.

- help

  - Prints list of commands.

- CacheAccess <address>

  - Access an address through the multilevel cache simulator.
  - Example: `CacheAccess 16`

- Cachestats

  - Print cache statistics for the multilevel cache.

- BuddyAlloc <size>

  - Allocate `<size>` bytes using the buddy allocator. Prints the returned pointer value on success.
  - Example: `BuddyAlloc 64`

- BuddyFree <address>

  - Free a buddy-allocated block by pointer. The program reads the pointer value (as text) from input.
  - Example (depending on printed pointer format): `BuddyFree 0x7ff...`
  - Note: parsing pointer values from the CLI depends on how the pointer was printed back.

- exit
  - Exit the simulator.

## Testing

This project includes a comprehensive test suite to validate memory allocation, cache behavior, and virtual address translation.

## Documentation

For comprehensive technical details, see:

- **Design Document** (`docs/DESIGN_DOCUMENT.md`) - Complete architecture, algorithms, and implementation details

## Extending

- Add more allocation strategies and enhance fragmentation metrics.
- Improve CLI pointer parsing for the buddy allocator (e.g., assign a symbolic id to each buddy allocation and free by id).
- Add more cache replacement policies (LRU, Random, etc.).
- Implement page-based virtual memory management.
- Add memory protection and access control mechanisms.
