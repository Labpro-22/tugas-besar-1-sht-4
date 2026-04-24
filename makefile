# Makefile for C++ OOP Project (Optimized & Recursive)

# Compiler settings
RAYLIB_PATH    ?= C:/raylib/raylib
RAYLIB_INCLUDE ?= $(RAYLIB_PATH)/src
RAYLIB_LIB     ?= $(RAYLIB_PATH)/src
TOOLCHAIN_BIN  ?= C:/raylib/w64devkit/bin

CXX      := $(TOOLCHAIN_BIN)/g++
CXXFLAGS := -Wall -Wextra -std=c++17 -B$(TOOLCHAIN_BIN)/ -Iinclude -I$(RAYLIB_INCLUDE)
LDFLAGS  := -L$(RAYLIB_LIB)
LDLIBS   := -lraylib -lopengl32 -lgdi32 -lwinmm

# Directories
SRC_DIR     := src
OBJ_DIR     := build
BIN_DIR     := bin
INCLUDE_DIR := include
DATA_DIR    := data
CONFIG_DIR  := config

# Target executable
TARGET := $(BIN_DIR)/game.exe

# 1. Recursive Source Finding
# Secara otomatis mencari semua file .cpp di dalam src/ dan semua sub-foldernya
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SRCS := $(call rwildcard,$(SRC_DIR)/,*.cpp)

# 2. Dynamic Object Mapping
# Mengubah path src/xxx/yyy.cpp menjadi build/xxx/yyy.o
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Main targets
all: directories $(TARGET)

# Create necessary root directories
directories:
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	@if not exist "$(DATA_DIR)" mkdir "$(DATA_DIR)"
	@if not exist "$(CONFIG_DIR)" mkdir "$(CONFIG_DIR)"

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)
	@echo "Build successful! Executable is at $(TARGET)"

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(subst /,\,$(patsubst %/,%,$(dir $@)))" mkdir "$(subst /,\,$(patsubst %/,%,$(dir $@)))"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the game
run: all
	$(subst /,\,$(TARGET))

# Clean up generated files
clean:
	@if exist "$(OBJ_DIR)" rmdir /S /Q "$(OBJ_DIR)"
	@if exist "$(BIN_DIR)" rmdir /S /Q "$(BIN_DIR)"
	@echo "Cleaned up $(OBJ_DIR) and $(BIN_DIR)"

# Rebuild everything from scratch
rebuild: clean all

.PHONY: all clean rebuild run directories
