# Modern OS Graphics System Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2 -I../libc/include
AR = ar
ARFLAGS = rcs

# Directories
GRAPHICS_DIR = graphics
INCLUDE_DIR = $(GRAPHICS_DIR)/include
SRC_DIR = $(GRAPHICS_DIR)/src

# GPU backend sources
GPU_SOURCES = $(wildcard $(SRC_DIR)/gpu/*.c)

# Graphics API sources
API_SOURCES = $(wildcard $(SRC_DIR)/api/*.c)

# Compositor sources
COMPOSITOR_SOURCES = $(wildcard $(SRC_DIR)/compositor/*.c)

# Font rendering sources
FONT_SOURCES = $(wildcard $(SRC_DIR)/font/*.c)

# All sources
ALL_SOURCES = $(GPU_SOURCES) $(API_SOURCES) $(COMPOSITOR_SOURCES) $(FONT_SOURCES)

# Objects
OBJECTS = $(ALL_SOURCES:.c=.o)

# Targets
GPU_LIB = $(GRAPHICS_DIR)/libgpu.a
GRAPHICS_LIB = $(GRAPHICS_DIR)/libgraphics.a
COMPOSITOR_LIB = $(GRAPHICS_DIR)/libcompositor.a
FONT_LIB = $(GRAPHICS_DIR)/libfont.a

.PHONY: all clean install

all: $(GPU_LIB) $(GRAPHICS_LIB) $(COMPOSITOR_LIB) $(FONT_LIB)

# GPU backend library
$(GPU_LIB): $(GPU_SOURCES:.c=.o)
	@mkdir -p $(GRAPHICS_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Graphics API library
$(GRAPHICS_LIB): $(API_SOURCES:.c=.o)
	@mkdir -p $(GRAPHICS_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Compositor library
$(COMPOSITOR_LIB): $(COMPOSITOR_SOURCES:.c=.o)
	@mkdir -p $(GRAPHICS_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Font library
$(FONT_LIB): $(FONT_SOURCES:.c=.o)
	@mkdir -p $(GRAPHICS_DIR)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(GPU_LIB) $(GRAPHICS_LIB) $(COMPOSITOR_LIB) $(FONT_LIB)

install: all
	mkdir -p /usr/local/lib
	cp $(GPU_LIB) $(GRAPHICS_LIB) $(COMPOSITOR_LIB) $(FONT_LIB) /usr/local/lib/
	mkdir -p /usr/local/include
	cp -r $(INCLUDE_DIR)/* /usr/local/include/
