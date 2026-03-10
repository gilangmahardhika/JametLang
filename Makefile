# Makefile untuk JametLang - Bahasa Pemrograman Basa Jawa

# Compiler
CC = gcc

# CFLAGS
CFLAGS = -Wall -Wextra -std=c11 -I./include

# LDFLAGS for linenoise (optional)
LDFLAGS =

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Target executables
TARGET_CLI = $(BIN_DIR)/jamet
TARGET_DEMO = $(BIN_DIR)/jametlang

# Source files for CLI
SOURCES_CLI = $(SRC_DIR)/jamet_types.c \
              $(SRC_DIR)/lexer.c \
              $(SRC_DIR)/repl.c \
              $(SRC_DIR)/cli.c

# Source files for Demo
SOURCES_DEMO = $(SRC_DIR)/jamet_types.c \
               $(SRC_DIR)/lexer.c \
               $(SRC_DIR)/main.c

# Object files for CLI
OBJECTS_CLI = $(BUILD_DIR)/jamet_types_cli.o \
              $(BUILD_DIR)/lexer_cli.o \
              $(BUILD_DIR)/repl.o \
              $(BUILD_DIR)/cli.o

# Object files for Demo
OBJECTS_DEMO = $(BUILD_DIR)/jamet_types_demo.o \
               $(BUILD_DIR)/lexer_demo.o \
               $(BUILD_DIR)/main.o

# Default target - build both
all: directories $(TARGET_CLI) $(TARGET_DEMO)

# Build CLI only
cli: directories $(TARGET_CLI)

# Build demo only
demo: directories $(TARGET_DEMO)

# Create directories if they don't exist
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# Link CLI object files
$(TARGET_CLI): $(OBJECTS_CLI)
	@echo "Nggabungke (Linking) Jamet CLI..."
	$(CC) $(OBJECTS_CLI) $(LDFLAGS) -o $(TARGET_CLI)
	@echo "Jamet CLI wis siap! Run dening: ./$(TARGET_CLI)"

# Link Demo object files
$(TARGET_DEMO): $(OBJECTS_DEMO)
	@echo "Nggabungke (Linking) Jamet Demo..."
	$(CC) $(OBJECTS_DEMO) -o $(TARGET_DEMO)
	@echo "Jamet Demo wis siap! Run dening: ./$(TARGET_DEMO)"

# Compile source files to object files
$(BUILD_DIR)/jamet_types_cli.o: $(SRC_DIR)/jamet_types.c
	@echo "Nggawe (Compiling) jamet_types.c..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lexer_cli.o: $(SRC_DIR)/lexer.c
	@echo "Nggawe (Compiling) lexer.c..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/repl.o: $(SRC_DIR)/repl.c
	@echo "Nggawe (Compiling) repl.c..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cli.o: $(SRC_DIR)/cli.c
	@echo "Nggawe (Compiling) cli.c..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/jamet_types_demo.o: $(SRC_DIR)/jamet_types.c
	@echo "Nggawe (Compiling) jamet_types.c (demo)..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lexer_demo.o: $(SRC_DIR)/lexer.c
	@echo "Nggawe (Compiling) lexer.c (demo)..."
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	@echo "Nggawe (Compiling) main.c..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	@echo "Beresna (Cleaning)..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Sampun!"

# Run CLI
run-cli: cli
	@echo "Mlancarake Jamet CLI..."
	./$(TARGET_CLI)

# Run Demo
run-demo: demo
	@echo "Mlancarake Jamet Demo..."
	./$(TARGET_DEMO)

# Rebuild
rebuild: clean all

# Install to /usr/local/bin (requires sudo)
install: cli
	@echo "Ninstal JametLang..."
	install -m 0755 $(TARGET_CLI) /usr/local/bin/jamet
	@echo "JametLang wis diinstal! Run 'jamet' saka ngendi wae."

# Uninstall from /usr/local/bin (requires sudo)
uninstall:
	@echo "Uninstal JametLang..."
	rm -f /usr/local/bin/jamet
	@echo "JametLang wis diuninstal."

.PHONY: all cli demo clean run-cli run-demo rebuild install uninstall directories
