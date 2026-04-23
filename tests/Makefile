# Modern OS Test Applications Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2 -I../libc/include -I../graphics/include -I../html/include
LDFLAGS = -L../libc -L../graphics/graphics -L../html/html
LIBS = -lc -lgpu -lgraphics -lhtml -lcss -ldom -ljs

# Directories
TESTS_DIR = tests
SRC_DIR = $(TESTS_DIR)/src
BIN_DIR = $(TESTS_DIR)/bin

# Test applications
KERNEL_TESTS = $(BIN_DIR)/kernel_test
LIBC_TESTS = $(BIN_DIR)/libc_test
GRAPHICS_TESTS = $(BIN_DIR)/graphics_test
HTML_TESTS = $(BIN_DIR)/html_test
BROWSER_TEST = $(BIN_DIR)/browser_test
BENCHMARK = $(BIN_DIR)/benchmark

# All tests
ALL_TESTS = $(KERNEL_TESTS) $(LIBC_TESTS) $(GRAPHICS_TESTS) $(HTML_TESTS) $(BROWSER_TEST) $(BENCHMARK)

.PHONY: all clean test kernel_test libc_test graphics_test html_test browser_test benchmark run_tests

all: $(ALL_TESTS)

# Create directories
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Kernel tests
$(KERNEL_TESTS): kernel_test.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lc

# Libc tests
$(LIBC_TESTS): libc_test.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lc

# Graphics tests
$(GRAPHICS_TESTS): graphics_test.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lgpu -lgraphics -lc

# HTML/CSS tests
$(HTML_TESTS): html_test.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lhtml -lcss -ldom -lc

# Browser test application
$(BROWSER_TEST): browser_test.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

# Performance benchmark
$(BENCHMARK): benchmark.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

# Test targets
kernel_test: $(KERNEL_TESTS)
	./$(KERNEL_TESTS)

libc_test: $(LIBC_TESTS)
	./$(LIBC_TESTS)

graphics_test: $(GRAPHICS_TESTS)
	./$(GRAPHICS_TESTS)

html_test: $(HTML_TESTS)
	./$(HTML_TESTS)

browser_test: $(BROWSER_TEST)
	./$(BROWSER_TEST)

benchmark: $(BENCHMARK)
	./$(BENCHMARK)

# Run all tests
run_tests: $(ALL_TESTS)
	@echo "Running all tests..."
	@echo "=================="
	@echo "Kernel tests:"
	./$(KERNEL_TESTS)
	@echo ""
	@echo "Libc tests:"
	./$(LIBC_TESTS)
	@echo ""
	@echo "Graphics tests:"
	./$(GRAPHICS_TESTS)
	@echo ""
	@echo "HTML/CSS tests:"
	./$(HTML_TESTS)
	@echo ""
	@echo "All tests completed successfully!"

# Performance testing
performance: $(BENCHMARK)
	@echo "Running performance benchmarks..."
	./$(BENCHMARK)

# Memory testing
memory_test: $(ALL_TESTS)
	@echo "Running memory tests with valgrind..."
	@for test in $(ALL_TESTS); do \
		echo "Testing $$test:"; \
		valgrind --leak-check=full --show-leak-kinds=all $$test; \
		echo ""; \
	done

# Code coverage
coverage: CFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: clean $(ALL_TESTS)
	@echo "Running tests with coverage..."
	./run_tests
	@echo "Generating coverage report..."
	gcov $(SRC_DIR)/*.c
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report

# Static analysis
analyze:
	@echo "Running static analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --error-exitcode=1 $(SRC_DIR)/; \
	else \
		echo "cppcheck not available, skipping analysis"; \
	fi

# Format code
format:
	@echo "Formatting test code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
	else \
		echo "clang-format not available, skipping formatting"; \
	fi

# Continuous integration
ci: clean all analyze run_tests memory_test performance

clean:
	rm -rf $(BIN_DIR)
	rm -f *.gcov *.gcda *.gcno coverage.info
	rm -rf coverage_report

# Help
help:
	@echo "Modern OS Test Suite"
	@echo "==================="
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all test applications"
	@echo "  kernel_test  - Run kernel tests"
	@echo "  libc_test    - Run libc tests"
	@echo "  graphics_test - Run graphics tests"
	@echo "  html_test    - Run HTML/CSS tests"
	@echo "  browser_test - Run browser test application"
	@echo "  benchmark    - Run performance benchmarks"
	@echo "  run_tests    - Run all tests"
	@echo "  performance  - Run performance tests"
	@echo "  memory_test  - Run tests with memory checking"
	@echo "  coverage     - Run tests with code coverage"
	@echo "  analyze      - Run static analysis"
	@echo "  format       - Format test code"
	@echo "  ci           - Run continuous integration pipeline"
	@echo "  clean        - Clean test build artifacts"
	@echo "  help         - Show this help message"
