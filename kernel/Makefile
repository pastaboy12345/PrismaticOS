# Modern OS Kernel Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -ffreestanding -nostdlib -fno-stack-protector
LDFLAGS = -nostdlib -T linker.ld -lgcc

# Directories
KERNEL_DIR = .
LIBC_DIR = ../libc
GRAPHICS_DIR = ../graphics
HTML_DIR = ../html

# Full kernel files with desktop environment
KERNEL_SOURCES = main.c console.c simple_mm.c simple_process.c simple_fs.c simple_interrupts.c simple_syscall.c desktop.c kernel_graphics.c kernel_html.c kernel_js.c
KERNEL_OBJECTS = $(KERNEL_SOURCES:.c=.o)
KERNEL_ASM = multiboot_simple.asm
KERNEL_ASM_OBJECTS = $(KERNEL_ASM:.asm=.o)

# Target
KERNEL_TARGET = kernel.bin

.PHONY: all clean kernel

all: kernel

kernel: $(KERNEL_TARGET)

$(KERNEL_TARGET): $(KERNEL_ASM_OBJECTS) $(KERNEL_OBJECTS)
	$(CC) -m32 -nostdlib -static -T linker_multiboot.ld -o $@ $^

%.o: %.c
	$(CC) -m32 $(CFLAGS) -I$(GRAPHICS_DIR)/include -I$(HTML_DIR)/include -I$(LIBC_DIR)/include -c $< -o $@

%.o: %.asm
	nasm -f elf32 $< -o $@

clean:
	rm -f $(KERNEL_OBJECTS) $(KERNEL_ASM_OBJECTS) $(KERNEL_TARGET)

# Create simple GRUB disk image and run
run: kernel
	./make_simple_disk.sh
	qemu-system-i386 -drive format=raw,file=os.img -m 512M

# Debug with GDB
debug: kernel
	qemu-system-x86_64 -kernel $(KERNEL_TARGET) -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_TARGET)"
