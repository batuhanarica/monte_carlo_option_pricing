# ============================================================================
# Monte Carlo Option Pricing - Makefile
# ============================================================================
# Build system for the Monte Carlo option pricing simulation.
# 
# Usage:
#   make          - Build the project (default)
#   make run      - Build and run the program
#   make test     - Build and run real stock tests
#   make debug    - Build with debug symbols
#   make clean    - Remove all build artifacts
#   make rebuild  - Clean and rebuild from scratch
# ============================================================================

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O3 -march=native -std=c11
LDFLAGS = -lm  # Link math library (needed for exp, sqrt, log, cos)

# Debug flags (used with 'make debug')
DEBUG_FLAGS = -g -O0 -DDEBUG

# Project structure
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TEST_DIR = tests

# Find all source files automatically (excluding main.c for library)
SRCS = $(wildcard $(SRC_DIR)/*.c)
LIB_SRCS = $(filter-out $(SRC_DIR)/main.c, $(SRCS))

# Generate object file names from source files
# src/main.c -> build/main.o
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIB_SRCS))

# Output executable name
TARGET = monte_carlo_option_pricing
TEST_TARGET = test_real_stocks

# ============================================================================
# Build Rules
# ============================================================================

# Default target: build the executable
all: $(BUILD_DIR) $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile each .c file into a .o file
# $< = source file, $@ = output file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I. -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ============================================================================
# Utility Targets
# ============================================================================

# Build and run the program
run: all
	@echo "Running $(TARGET)..."
	@./$(TARGET)

# ============================================================================
# Test Targets
# ============================================================================

# Build the test executable
$(TEST_TARGET): $(LIB_OBJS) $(BUILD_DIR)/test_real_stocks.o
	@echo "Linking $(TEST_TARGET)..."
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Test build complete: $(TEST_TARGET)"

# Compile test file
$(BUILD_DIR)/test_real_stocks.o: $(TEST_DIR)/test_real_stocks.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I. -c $< -o $@

# Build and run tests with real stock data
test: $(BUILD_DIR) $(LIB_OBJS) $(TEST_TARGET)
	@echo "Running real stock tests..."
	@./$(TEST_TARGET) $(TEST_DIR)/real_stocks.csv

# Run tests with custom simulation count
test-fast: $(BUILD_DIR) $(LIB_OBJS) $(TEST_TARGET)
	@echo "Running fast tests (100k simulations)..."
	@./$(TEST_TARGET) $(TEST_DIR)/real_stocks.csv 100000

test-accurate: $(BUILD_DIR) $(LIB_OBJS) $(TEST_TARGET)
	@echo "Running accurate tests (2M simulations)..."
	@./$(TEST_TARGET) $(TEST_DIR)/real_stocks.csv 2000000

# Run tests with random seed (different results each time)
test-random: $(BUILD_DIR) $(LIB_OBJS) $(TEST_TARGET)
	@echo "Running tests with random seed..."
	@./$(TEST_TARGET) $(TEST_DIR)/real_stocks.csv 500000 --random

# Build with debug symbols (for gdb, valgrind, etc.)
debug: CFLAGS = $(DEBUG_FLAGS)
debug: clean all
	@echo "Debug build complete"

# Remove all build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
	@echo "Clean complete"

# Clean and rebuild everything
rebuild: clean all

# ============================================================================
# Development Helpers
# ============================================================================

# Check for memory leaks (requires valgrind)
memcheck: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Show all source files that will be compiled
info:
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"
	@echo "Target: $(TARGET)"

# Phony targets (not actual files)
.PHONY: all run debug clean rebuild memcheck info test test-fast test-accurate test-random
