# Makefile untuk JametLang - Bahasa Pemrograman Basa Jawa

# Compiler
CC = gcc

# CFLAGS
CFLAGS = -Wall -Wextra -std=c11 -I./include

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/jametlang

# Source files
SOURCES = $(SRC_DIR)/jamet_types.c \
          $(SRC_DIR)/lexer.c \
          $(SRC_DIR)/main.c

# Object files
OBJECTS = $(BUILD_DIR)/jamet_types.o \
          $(BUILD_DIR)/lexer.o \
          $(BUILD_DIR)/main.o

# Default target
all: directories $(TARGET)

# Create directories if they don't exist
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	@echo "Nggabungke (Linking) JametLang..."
	$(CC) $(OBJECTS) -o $(TARGET)
	@echo "JametLang wis siap! Run dening: ./$(TARGET)"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Nggawe (Compiling) $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	@echo "Beresna (Cleaning)..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Sampun!"

# Run the program
run: all
	@echo "Mlancarake JametLang..."
	./$(TARGET)

# Rebuild
rebuild: clean all

.PHONY: all clean run rebuild directories
