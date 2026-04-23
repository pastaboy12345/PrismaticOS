# Modern OS Project Root Makefile
.PHONY: all clean kernel libc graphics html test run debug install

# Default target
all: kernel libc graphics

# Kernel
kernel:
	@echo "Building kernel..."
	$(MAKE) -C kernel

# C library
libc:
	@echo "Building libc..."
	$(MAKE) -C libc

# Graphics system
graphics:
	@echo "Building graphics system..."
	$(MAKE) -C graphics

# HTML/CSS engine
html:
	@echo "Building HTML/CSS engine..."
	$(MAKE) -C html

# Test applications
test: all
	@echo "Building test applications..."
	$(MAKE) -C tests

# Clean all
clean:
	@echo "Cleaning all..."
	$(MAKE) -C kernel clean
	$(MAKE) -C libc clean
	$(MAKE) -C graphics clean
	$(MAKE) -C html clean
	$(MAKE) -C tests clean

# Install
install: all
	@echo "Installing..."
	$(MAKE) -C libc install
	$(MAKE) -C graphics install

# Run with QEMU
run: kernel
	@echo "Running kernel with QEMU..."
	$(MAKE) -C kernel run

# Debug with GDB
debug: kernel
	@echo "Debugging kernel with GDB..."
	$(MAKE) -C kernel debug

# Build everything
everything: all html test

# Help
help:
	@echo "Modern OS Build System"
	@echo "===================="
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build kernel, libc, and graphics"
	@echo "  kernel   - Build kernel only"
	@echo "  libc     - Build C library only"
	@echo "  graphics - Build graphics system only"
	@echo "  html     - Build HTML/CSS engine"
	@echo "  test     - Build test applications"
	@echo "  clean    - Clean all build artifacts"
	@echo "  install  - Install libraries and headers"
	@echo "  run      - Run kernel with QEMU"
	@echo "  debug    - Debug kernel with GDB"
	@echo "  everything - Build everything"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make                # Build all core components"
	@echo "  make kernel         # Build just the kernel"
	@echo "  make run            # Build and run kernel"
	@echo "  make clean all      # Clean then rebuild"

# Show build status
status:
	@echo "Build Status:"
	@echo "============"
	@if [ -f kernel/kernel.bin ]; then \
		echo "  Kernel:    Built"; \
	else \
		echo "  Kernel:    Not built"; \
	fi
	@if [ -f libc/libc.a ]; then \
		echo "  Libc:      Built"; \
	else \
		echo "  Libc:      Not built"; \
	fi
	@if [ -f graphics/graphics/libgpu.a ]; then \
		echo "  Graphics:  Built"; \
	else \
		echo "  Graphics:  Not built"; \
	fi
	@if [ -f html/html/libhtml.a ]; then \
		echo "  HTML:      Built"; \
	else \
		echo "  HTML:      Not built"; \
	fi

# Quick rebuild (clean and build)
rebuild: clean all

# Create distribution
dist: clean everything
	@echo "Creating distribution..."
	@mkdir -p dist
	@tar -czf dist/modern-os-$(shell date +%Y%m%d).tar.gz \
		--exclude=dist \
		--exclude=.git \
		--exclude='*.o' \
		--exclude='*.a' \
		--exclude='*.bin' \
		.
	@echo "Distribution created in dist/"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@which gcc > /dev/null || (echo "Error: gcc not found" && exit 1)
	@which nasm > /dev/null || (echo "Error: nasm not found" && exit 1)
	@which ar > /dev/null || (echo "Error: ar not found" && exit 1)
	@which qemu-system-x86_64 > /dev/null || echo "Warning: qemu-system-x86_64 not found (run target won't work)"
	@echo "Dependencies OK"

# Generate documentation
docs:
	@echo "Generating documentation..."
	@if [ -d docs ]; then rm -rf docs; fi
	@mkdir -p docs
	@doxygen kernel/Doxyfile 2>/dev/null || echo "Doxygen not available, skipping kernel docs"
	@echo "Documentation generated in docs/"

# Static analysis
analyze:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --error-exitcode=1 kernel/ libc/ graphics/; \
	else \
		echo "cppcheck not available, skipping analysis"; \
	fi

# Performance test
benchmark: test
	@echo "Running benchmarks..."
	@cd tests && ./benchmark

# Format code
format:
	@echo "Formatting code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find . -name "*.c" -o -name "*.h" | xargs clang-format -i; \
	else \
		echo "clang-format not available, skipping formatting"; \
	fi

# Check code style
style:
	@echo "Checking code style..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find . -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror; \
	else \
		echo "clang-format not available, skipping style check"; \
	fi

# Continuous integration target
ci: check-deps clean everything test analyze style
	@echo "CI pipeline completed successfully"
