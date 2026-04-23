# Modern OS HTML/CSS Engine Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2 -I../libc/include -I../graphics/include
AR = ar
ARFLAGS = rcs

# Directories
HTML_DIR = html
INCLUDE_DIR = $(HTML_DIR)/include
SRC_DIR = $(HTML_DIR)/src

# HTML parser sources
HTML_SOURCES = $(wildcard $(SRC_DIR)/html/*.c)

# CSS parser sources
CSS_SOURCES = $(wildcard $(SRC_DIR)/css/*.c)

# Layout engine sources
LAYOUT_SOURCES = $(wildcard $(SRC_DIR)/layout/*.c)

# Rendering sources
RENDER_SOURCES = $(wildcard $(SRC_DIR)/render/*.c)

# DOM sources
DOM_SOURCES = $(wildcard $(SRC_DIR)/dom/*.c)

# JavaScript integration sources
JS_SOURCES = $(wildcard $(SRC_DIR)/js/*.c)

# All sources
ALL_SOURCES = $(HTML_SOURCES) $(CSS_SOURCES) $(LAYOUT_SOURCES) $(RENDER_SOURCES) $(DOM_SOURCES) $(JS_SOURCES)

# Objects
OBJECTS = $(ALL_SOURCES:.c=.o)

# Targets
HTML_LIB = $(HTML_DIR)/libhtml.a
CSS_LIB = $(HTML_DIR)/libcss.a
LAYOUT_LIB = $(HTML_DIR)/liblayout.a
RENDER_LIB = $(HTML_DIR)/librender.a
DOM_LIB = $(HTML_DIR)/libdom.a
JS_LIB = $(HTML_DIR)/libjs.a

.PHONY: all clean install

all: $(HTML_LIB) $(CSS_LIB) $(LAYOUT_LIB) $(RENDER_LIB) $(DOM_LIB) $(JS_LIB)

# HTML parser library
$(HTML_LIB): $(HTML_SOURCES:.c=.o)
	@mkdir -p $(HTML_DIR)
	$(AR) $(ARFLAGS) $@ $^

# CSS parser library
$(CSS_LIB): $(CSS_SOURCES:.c=.o)
	@mkdir -p $(HTML_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Layout engine library
$(LAYOUT_LIB): $(LAYOUT_SOURCES:.c=.o)
	@mkdir -p $(HTML_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Rendering library
$(RENDER_LIB): $(RENDER_SOURCES:.c=.o)
	@mkdir -p $(HTML_DIR)
	$(AR) $(ARFLAGS) $@ $^

# DOM library
$(DOM_LIB): $(DOM_SOURCES:.c=.o)
	@mkdir -p $(HTML_DIR)
	$(AR) $(ARFLAGS) $@ $^

# JavaScript integration library
$(JS_LIB): $(JS_SOURCES:.c=.o)
	@mkdir -p $(HTML_DIR)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(HTML_LIB) $(CSS_LIB) $(LAYOUT_LIB) $(RENDER_LIB) $(DOM_LIB) $(JS_LIB)

install: all
	mkdir -p /usr/local/lib
	cp $(HTML_LIB) $(CSS_LIB) $(LAYOUT_LIB) $(RENDER_LIB) $(DOM_LIB) $(JS_LIB) /usr/local/lib/
	mkdir -p /usr/local/include
	cp -r $(INCLUDE_DIR)/* /usr/local/include/
