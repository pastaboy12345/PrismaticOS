# Modern OS libc Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -ffreestanding -nostdlib -fno-stack-protector -O2
AR = ar
ARFLAGS = rcs

# Directories
LIBC_DIR = libc
INCLUDE_DIR = $(LIBC_DIR)/include
SRC_DIR = $(LIBC_DIR)/src

# Source files
STDLIB_SOURCES = $(wildcard $(SRC_DIR)/stdlib/*.c)
STDIO_SOURCES = $(wildcard $(SRC_DIR)/stdio/*.c)
STRING_SOURCES = $(wildcard $(SRC_DIR)/string/*.c)
UNISTD_SOURCES = $(wildcard $(SRC_DIR)/unistd/*.c)
SYSCALL_SOURCES = $(wildcard $(SRC_DIR)/syscall/*.c)
MATH_SOURCES = $(wildcard $(SRC_DIR)/math/*.c)
TIME_SOURCES = $(wildcard $(SRC_DIR)/time/*.c)
CTYPE_SOURCES = $(wildcard $(SRC_DIR)/ctype/*.c)
STDARG_SOURCES = $(wildcard $(SRC_DIR)/stdarg/*.c)

ALL_SOURCES = $(STDLIB_SOURCES) $(STDIO_SOURCES) $(STRING_SOURCES) $(UNISTD_SOURCES) \
               $(SYSCALL_SOURCES) $(MATH_SOURCES) $(TIME_SOURCES) $(CTYPE_SOURCES) \
               $(STDARG_SOURCES)

OBJECTS = $(ALL_SOURCES:.c=.o)

# Target
LIBC_TARGET = $(LIBC_DIR)/libc.a

.PHONY: all clean install

all: $(LIBC_TARGET)

$(LIBC_TARGET): $(OBJECTS)
	@mkdir -p $(LIBC_DIR)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBC_TARGET)

install: $(LIBC_TARGET)
	mkdir -p /usr/local/lib
	cp $(LIBC_TARGET) /usr/local/lib/
	mkdir -p /usr/local/include
	cp -r $(INCLUDE_DIR)/* /usr/local/include/
