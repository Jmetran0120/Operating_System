# Makefile for JoshOS
#
# Builds the operating system from source files into a bootable ISO.
# Usage: make (builds ISO), make run (builds and runs in QEMU), make clean

# Suppress implicit rules to prevent conflicts
.SUFFIXES:

# Compiler and tools
# On macOS, install cross-compiler: brew install i686-elf-gcc i686-elf-binutils
# Auto-detect cross-compiler if available
CC := $(shell command -v i686-elf-gcc 2>/dev/null || echo gcc)
AS := $(shell command -v i686-elf-as 2>/dev/null || echo as)
LD := $(shell command -v i686-elf-ld 2>/dev/null || echo ld)
# GRUB tool - try i686-elf-grub-mkrescue first (for macOS), fallback to grub-mkrescue
GRUB_MKRESCUE ?= $(shell command -v i686-elf-grub-mkrescue 2>/dev/null || command -v grub-mkrescue 2>/dev/null || echo grub-mkrescue)

# Check if tools exist (basic check)
CHECK_CC := $(shell command -v $(CC) 2>/dev/null)
CHECK_AS := $(shell command -v $(AS) 2>/dev/null)
CHECK_LD := $(shell command -v $(LD) 2>/dev/null)

# Warn if tools not found (only check on first run, not on every invocation)
ifeq ($(MAKELEVEL),0)
ifndef CHECK_CC
$(warning WARNING: $(CC) not found in PATH. Install with: brew install i686-elf-gcc)
endif
ifndef CHECK_AS
$(warning WARNING: $(AS) not found in PATH. Install with: brew install i686-elf-binutils)
endif
ifndef CHECK_LD
$(warning WARNING: $(LD) not found in PATH. Install with: brew install i686-elf-binutils)
endif
endif

# Compiler flags
CFLAGS = -m32                      # 32-bit mode
CFLAGS += -std=c11                 # C11 standard
CFLAGS += -ffreestanding           # No standard library (OS kernel)
CFLAGS += -O2                      # Optimization level 2
CFLAGS += -Wall                    # All warnings
CFLAGS += -Wextra                  # Extra warnings
CFLAGS += -nostdlib                # No standard library
CFLAGS += -fno-builtin             # No built-in functions
CFLAGS += -fno-stack-protector     # No stack protection (not needed in kernel)
CFLAGS += -fno-pic                 # No position-independent code
CFLAGS += -fno-pie                 # No position-independent executable

# Assembler flags
# Note: x86_64-elf-as doesn't support --32 flag (it's for 64-bit)
# For 32-bit OS, you MUST use i686-elf toolchain: brew install i686-elf-gcc i686-elf-binutils
# Check if we're using x86_64-elf-as (which doesn't work for 32-bit)
ifeq ($(findstring x86_64-elf-as,$(AS)),x86_64-elf-as)
$(error ERROR: x86_64-elf-as cannot assemble 32-bit code. Please install i686-elf toolchain: brew install i686-elf-gcc i686-elf-binutils)
endif
# Assembler flags - only use --32 for cross-compiler
ASFLAGS = 
ifeq ($(findstring i686-elf-as,$(AS)),i686-elf-as)
    ASFLAGS += --32
endif

# Linker flags
LDFLAGS = -T linker.ld            # Use our linker script
LDFLAGS += -nostdlib               # No standard library
# Add -m elf_i386 if using cross-compiler (i686-elf-ld)
ifeq ($(findstring i686-elf-ld,$(LD)),i686-elf-ld)
    LDFLAGS += -m elf_i386
endif

# Directories
SRC_DIR := src
ISO_DIR := iso
BUILD_DIR := build

# Source files
BOOT_SRC := $(SRC_DIR)/boot.S
KERNEL_SRC := $(SRC_DIR)/kernel.c
KEYBOARD_SRC := $(SRC_DIR)/keyboard.c
MEMORY_SRC := $(SRC_DIR)/memory.c
GRAPHICS_SRC := $(SRC_DIR)/graphics.c
NEBULA_UI_SRC := $(SRC_DIR)/nebula_ui.c

# Object files
BOOT_OBJ := $(BUILD_DIR)/boot.o
KERNEL_OBJ := $(BUILD_DIR)/kernel.o
KEYBOARD_OBJ := $(BUILD_DIR)/keyboard.o
MEMORY_OBJ := $(BUILD_DIR)/memory.o
GRAPHICS_OBJ := $(BUILD_DIR)/graphics.o
NEBULA_UI_OBJ := $(BUILD_DIR)/nebula_ui.o

# Output files
KERNEL_BIN = $(ISO_DIR)/boot/kernel.bin  # Kernel binary
ISO_FILE = JoshOS.iso                     # Final ISO file

# Default target - build ISO
all: $(ISO_FILE)

# Build ISO file
$(ISO_FILE): $(KERNEL_BIN)
	@echo "Creating ISO..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@printf 'menuentry "NEBULA OS" {\n    multiboot /boot/kernel.bin\n    boot\n}\n' > $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO_FILE) $(ISO_DIR)

# Build kernel binary
$(KERNEL_BIN): $(BUILD_DIR)/kernel.bin
	@echo "Copying kernel to ISO directory..."
	@mkdir -p $(ISO_DIR)/boot
	cp $(BUILD_DIR)/kernel.bin $(KERNEL_BIN)

# Link kernel binary from object files
$(BUILD_DIR)/kernel.bin: $(BOOT_OBJ) $(KERNEL_OBJ) $(KEYBOARD_OBJ) $(MEMORY_OBJ) $(GRAPHICS_OBJ) $(NEBULA_UI_OBJ)
	@echo "Linking kernel..."
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel.bin $(BOOT_OBJ) $(KERNEL_OBJ) $(KEYBOARD_OBJ) $(MEMORY_OBJ) $(GRAPHICS_OBJ) $(NEBULA_UI_OBJ)

# Compile bootloader
$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.S
	@echo "Assembling bootloader..."
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $(BUILD_DIR)/boot.o $(SRC_DIR)/boot.S

# Compile kernel
$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c
	@echo "Compiling kernel..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(BUILD_DIR)/kernel.o $(SRC_DIR)/kernel.c

# Compile keyboard driver
$(BUILD_DIR)/keyboard.o: $(SRC_DIR)/keyboard.c
	@echo "Compiling keyboard driver..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(BUILD_DIR)/keyboard.o $(SRC_DIR)/keyboard.c

# Compile memory manager
$(BUILD_DIR)/memory.o: $(SRC_DIR)/memory.c
	@echo "Compiling memory manager..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(BUILD_DIR)/memory.o $(SRC_DIR)/memory.c

# Compile graphics subsystem
$(BUILD_DIR)/graphics.o: $(SRC_DIR)/graphics.c
	@echo "Compiling graphics subsystem..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(BUILD_DIR)/graphics.o $(SRC_DIR)/graphics.c

# Compile NEBULA UI
$(BUILD_DIR)/nebula_ui.o: $(SRC_DIR)/nebula_ui.c
	@echo "Compiling NEBULA UI..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(BUILD_DIR)/nebula_ui.o $(SRC_DIR)/nebula_ui.c

# Run in QEMU
# Try to find qemu-system-i386 in common locations
QEMU := $(shell command -v qemu-system-i386 2>/dev/null || find /usr/local /opt/homebrew -name qemu-system-i386 2>/dev/null | head -1 || echo qemu-system-i386)

run: $(ISO_FILE)
	@echo "Starting QEMU..."
	@if [ ! -f JoshOS.iso ]; then \
		echo "Error: ISO file not found. Run 'make' first."; \
		exit 1; \
	fi
	$(QEMU) -cdrom JoshOS.iso -m 128M

# Clean build artifacts
clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)/boot/kernel.bin
	rm -f $(ISO_FILE)
	@mkdir -p $(ISO_DIR)/boot/grub
	@printf 'menuentry "NEBULA OS" {\n    multiboot /boot/kernel.bin\n    boot\n}\n' > $(ISO_DIR)/boot/grub/grub.cfg

# Phony targets (not files)
.PHONY: all run clean
