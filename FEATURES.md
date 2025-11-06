# Adding Keyboard Input and Memory Management to JoshOS

This document explains how keyboard input and memory management features were added to JoshOS.

## Keyboard Input Implementation

### Overview
The keyboard driver reads PS/2 keyboard scan codes and converts them to ASCII characters that can be used by the kernel.

### Step 1: Create Keyboard Driver Files

**keyboard.h** - Header file with function declarations:
```c
uint8_t keyboard_has_data(void);      // Check if key is available
char keyboard_getchar(void);           // Get single character
void keyboard_readline(char* buffer, uint32_t max_len); // Read line
```

**keyboard.c** - Implementation:

#### Port I/O Function
```c
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
```
- Uses inline assembly to read from I/O ports
- `inb` instruction reads a byte from the specified port
- Port 0x60 = keyboard data, Port 0x64 = keyboard status

#### Scan Code to ASCII Mapping
```c
static const char scan_code_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', ...
};
```
- Maps raw scan codes (0-127) to ASCII characters
- Handles special keys: Backspace ('\b'), Enter ('\n'), Tab ('\t')

#### Reading Characters
```c
char keyboard_getchar(void) {
    while (!keyboard_has_data()) { }  // Wait for key
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    if (scancode & KEY_RELEASE_MASK) return 0;  // Ignore releases
    return scan_code_to_ascii[scancode];
}
```
- Checks status port (0x64) for data availability
- Reads scan code from data port (0x60)
- Ignores key releases (bit 7 set)
- Converts to ASCII

#### Line Input Function
```c
void keyboard_readline(char* buffer, uint32_t max_len) {
    buffer_index = 0;
    while (buffer_index < max_len - 1) {
        char c = keyboard_getchar();
        if (c == '\n') { buffer[buffer_index] = '\0'; return; }
        if (c == '\b') { /* handle backspace */ }
        buffer[buffer_index++] = c;
        terminal_putchar(c);  // Echo to screen
    }
}
```
- Reads characters until Enter is pressed
- Handles backspace for editing
- Echoes characters to screen
- Null-terminates the string

### Step 2: Integrate into Kernel

In **kernel.c**:
```c
#include "keyboard.h"

void kernel_main(void) {
    // ... initialization ...
    
    char input[256];
    while (1) {
        terminal_writestring("JoshOS> ");
        keyboard_readline(input, sizeof(input));  // Read command
        // Process command...
    }
}
```

### How It Works
1. **Hardware**: Keyboard controller sends scan codes to port 0x60
2. **Polling**: Kernel checks status port (0x64) for available data
3. **Translation**: Scan codes converted to ASCII via lookup table
4. **Echo**: Characters displayed on screen as typed
5. **Buffer**: Complete line stored in buffer when Enter pressed

---

## Memory Management Implementation

### Overview
A simple heap allocator using a linked list of memory blocks. Each block has a header containing size, free flag, and next pointer.

### Step 1: Create Memory Manager Files

**memory.h** - Header file:
```c
void memory_init(void);                    // Initialize heap
void* kmalloc(uint32_t size);              // Allocate memory
void kfree(void* ptr);                     // Free memory
void* kcalloc(uint32_t num, uint32_t size); // Allocate and zero
void* krealloc(void* ptr, uint32_t size);  // Reallocate
```

**memory.c** - Implementation:

#### Memory Block Structure
```c
typedef struct MemoryBlock {
    struct MemoryBlock* next;  // Next block in list
    uint32_t size;             // Size of data area
    uint8_t free;              // 1=free, 0=allocated
} MemoryBlock;
```
- Header precedes each memory block
- Linked list connects all blocks
- Free flag indicates allocation status

#### Heap Initialization
```c
#define HEAP_START 0x1000000   // Start at 16MB
#define HEAP_SIZE  0x100000    // 1MB heap

void memory_init(void) {
    heap_start->next = NULL;
    heap_start->size = HEAP_SIZE - sizeof(MemoryBlock);
    heap_start->free = 1;
}
```
- Creates one large free block covering entire heap
- Heap located at 16MB (safe memory area)

#### Allocation Algorithm
```c
void* kmalloc(uint32_t size) {
    size = (size + 3) & ~3;  // Align to 4 bytes
    
    MemoryBlock* current = heap_start;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            // Split block if large enough
            if (current->size >= size + sizeof(MemoryBlock) + 4) {
                MemoryBlock* new = /* create new block */;
                new->size = current->size - size - sizeof(MemoryBlock);
                new->free = 1;
                current->next = new;
            }
            current->size = size;
            current->free = 0;
            return (void*)((uint8_t*)current + sizeof(MemoryBlock));
        }
        current = current->next;
    }
    return NULL;  // Out of memory
}
```

**First-Fit Algorithm**:
1. Search from heap start
2. Find first free block large enough
3. Split block if significantly larger than needed
4. Mark as allocated
5. Return pointer to data area (after header)

#### Freeing Memory
```c
void kfree(void* ptr) {
    MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
    block->free = 1;
    
    // Merge with next block if free
    if (block->next != NULL && block->next->free) {
        block->size += sizeof(MemoryBlock) + block->next->size;
        block->next = block->next->next;
    }
    
    // Merge with previous block if free
    // (similar code for previous block)
}
```

**Coalescing**:
- Mark block as free
- Check if next block is free → merge
- Check if previous block is free → merge
- Prevents fragmentation

### Step 2: Integrate into Kernel

In **kernel.c**:
```c
#include "memory.h"

void kernel_main(void) {
    memory_init();  // Initialize heap
    
    // Use memory manager
    void* ptr = kmalloc(100);
    kfree(ptr);
}
```

### Memory Layout

```
Heap Start (16MB)
├── Block 1 Header
│   ├── next: → Block 2
│   ├── size: 50000
│   └── free: 0 (allocated)
├── Block 1 Data (50000 bytes)
├── Block 2 Header
│   ├── next: → Block 3
│   ├── size: 20000
│   └── free: 1 (free)
├── Block 2 Data (20000 bytes)
└── ...
```

### Key Concepts

1. **Block Splitting**: Large blocks split to minimize waste
2. **Block Merging**: Adjacent free blocks combined to reduce fragmentation
3. **Alignment**: All allocations aligned to 4-byte boundaries
4. **First-Fit**: Simple algorithm - first suitable block is used
5. **Header Overhead**: Each block has ~12 bytes overhead

### Limitations

- No protection against double-free (can be added)
- No bounds checking (can be added)
- Simple first-fit (fragmentation possible)
- Fixed heap size (1MB)
- No virtual memory (direct physical addresses)

### Improvements Possible

- **Best-Fit**: Find smallest suitable block
- **Buddy System**: Power-of-2 block sizes
- **Paging**: Virtual memory support
- **Guard Pages**: Detect buffer overflows
- **Free List**: Separate list for free blocks (faster)

---

## Integration Steps Summary

1. **Create driver files** (keyboard.c, keyboard.h, memory.c, memory.h)
2. **Update Makefile** to compile new source files
3. **Include headers** in kernel.c
4. **Initialize** in kernel_main()
5. **Use in main loop** (keyboard) or allocate (memory)

## Testing

### Keyboard Testing
- Type characters → should appear on screen
- Press Enter → command should execute
- Backspace → should delete characters
- Commands → should be recognized

### Memory Testing
- Run `mem` command → shows statistics
- Run `test` command → allocates/frees blocks
- Check statistics → should show used/free memory

## Common Issues

### Keyboard Not Working
- Check port numbers (0x60, 0x64)
- Verify scan code table is correct
- Ensure interrupts aren't interfering (if using interrupts later)

### Memory Allocation Fails
- Check heap initialization
- Verify block splitting logic
- Ensure heap size is sufficient
- Check for memory corruption (overwrites)

### Build Errors
- Ensure all source files in Makefile
- Check include paths (-I$(SRC_DIR))
- Verify function declarations match definitions

