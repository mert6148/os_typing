# Makefile for OS Typing Project

# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11 -O2 -fPIC
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -fPIC
LDFLAGS = -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)

# Targets
TARGET = $(BIN_DIR)/os_typing

.PHONY: all build clean test debug release help

# Default target
all: build

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/test_%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link executable
build: $(BIN_DIR) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Run tests
test: $(TEST_OBJS)
	@for test in $(TEST_SRCS); do \
		test_bin=$(BUILD_DIR)/$$(basename $$test .c); \
		$(CC) $(CFLAGS) -o $$test_bin $$test $(OBJS) $(LDFLAGS); \
		echo "Running $$test_bin..."; \
		$$test_bin; \
	done

# Debug build
debug: CFLAGS += -g -O0 -DDEBUG
debug: build

# Release build
release: CFLAGS += -O3 -DNDEBUG
release: clean build

# Memory check with Valgrind
memcheck: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(TARGET)

# Static analysis with clang
analyze:
	clang --analyze $(SRCS)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	find . -name "*.o" -delete
	find . -name "*.plist" -delete

# Install
install: build
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/
	@echo "Installed to /usr/local/bin/"

# Uninstall
uninstall:
	rm -f /usr/local/bin/os_typing

# Help
help:
	@echo "Available targets:"
	@echo "  make all       - Build project"
	@echo "  make build     - Build with optimization"
	@echo "  make debug     - Build with debug symbols"
	@echo "  make release   - Build optimized release"
	@echo "  make test      - Run unit tests"
	@echo "  make memcheck  - Run with Valgrind"
	@echo "  make analyze   - Run static analysis"
	@echo "  make clean     - Remove build artifacts"
	@echo "  make install   - Install binary"
	@echo "  make uninstall - Uninstall binary"
	@echo "  make help      - Show this help"

.PHONY: memcheck analyze install uninstall
