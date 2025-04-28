# Compiler and tools
CC ?= gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g -Iinclude
VALGRIND = valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = tests
EXAMPLES_DIR = examples

# Sources and object files (only src/)
LIB_SRCS = $(wildcard $(SRC_DIR)/*.c)
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIB_SRCS))

# Test and example programs
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
EXAMPLE_SRCS = $(wildcard $(EXAMPLES_DIR)/*.c)

TEST_BIN = $(BUILD_DIR)/test_runner
EXAMPLE_BINS = $(patsubst $(EXAMPLES_DIR)/%.c, $(BUILD_DIR)/example_%, $(EXAMPLE_SRCS))

# Default target: just compile object files
all: $(BUILD_DIR) $(LIB_OBJS)

# Build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile .o files from src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test program (linked against compiled ADT objects)
test: all $(TEST_BIN)

$(TEST_BIN): $(LIB_OBJS) $(TEST_SRCS)
	$(CC) $(CFLAGS) $^ -o $@

# Build examples (linked against compiled ADT objects)
examples: all $(EXAMPLE_BINS)

$(BUILD_DIR)/example_%: $(EXAMPLES_DIR)/%.c $(LIB_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Run tests
runtest: test
	./$(TEST_BIN)

# Run tests with Valgrind
memcheck: test
	$(VALGRIND) ./$(TEST_BIN)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean test examples runtest memcheck
