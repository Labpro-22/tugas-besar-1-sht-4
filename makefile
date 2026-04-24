# Makefile for C++ OOP Project (Optimized & Recursive)

# Platform/compiler settings
ifeq ($(OS),Windows_NT)
HOST_PLATFORM := windows
else
WSL_DETECTED := $(shell grep -qi microsoft /proc/version 2>/dev/null && echo yes)
ifeq ($(WSL_DETECTED),yes)
HOST_PLATFORM := wsl
else
HOST_PLATFORM := linux
endif
endif

ifeq ($(HOST_PLATFORM),windows)
RAYLIB_PATH    ?= C:/raylib/raylib
RAYLIB_INCLUDE ?= $(RAYLIB_PATH)/src
RAYLIB_LIB     ?= $(RAYLIB_PATH)/src
TOOLCHAIN_BIN  ?= C:/raylib/w64devkit/bin
TOOLCHAIN_PREFIX ?= $(TOOLCHAIN_BIN)

CXX      := $(TOOLCHAIN_BIN)/g++
CXXFLAGS := -Wall -Wextra -std=c++17 -B$(TOOLCHAIN_PREFIX)/ -Iinclude -I$(RAYLIB_INCLUDE)
LDFLAGS  := -L$(RAYLIB_LIB)
LDLIBS   := -lraylib -lopengl32 -lgdi32 -lwinmm
EXEEXT   := .exe
RUN_CMD  := $(subst /,\,$(BIN_DIR)/game$(EXEEXT))
STATIC_RUNTIME := -static-libgcc -static-libstdc++
define MKDIR_CMD
if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
endef
define RM_CMD
if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
endef
else ifeq ($(HOST_PLATFORM),wsl)
RAYLIB_PATH    ?= C:/raylib/raylib
RAYLIB_INCLUDE ?= $(RAYLIB_PATH)/src
RAYLIB_LIB     ?= $(RAYLIB_PATH)/src
TOOLCHAIN_BIN  ?= /mnt/c/raylib/w64devkit/bin
TOOLCHAIN_PREFIX ?= C:/raylib/w64devkit/bin

CXX      := cmd.exe /C C:/raylib/w64devkit/bin/g++.exe
CXXFLAGS := -Wall -Wextra -std=c++17 -B$(TOOLCHAIN_PREFIX)/ -Iinclude -I$(RAYLIB_INCLUDE)
LDFLAGS  := -L$(RAYLIB_LIB)
LDLIBS   := -lraylib -lopengl32 -lgdi32 -lwinmm
EXEEXT   := .exe
RUN_CMD  := ./$(BIN_DIR)/game$(EXEEXT)
STATIC_RUNTIME := -static-libgcc -static-libstdc++
define MKDIR_CMD
mkdir -p "$(1)"
endef
define RM_CMD
rm -rf "$(1)"
endef
else
RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS   := $(shell pkg-config --libs raylib 2>/dev/null)

CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude $(RAYLIB_CFLAGS)
LDFLAGS  :=
LDLIBS   := $(if $(strip $(RAYLIB_LIBS)),$(RAYLIB_LIBS),-lraylib -lGL -lm -lpthread -ldl -lrt -lX11)
EXEEXT   :=
RUN_CMD  := ./$(BIN_DIR)/game$(EXEEXT)
STATIC_RUNTIME :=
define MKDIR_CMD
mkdir -p "$(1)"
endef
define RM_CMD
rm -rf "$(1)"
endef
endif

# Directories
SRC_DIR     := src
OBJ_DIR     := build
BIN_DIR     := bin
INCLUDE_DIR := include
DATA_DIR    := data
CONFIG_DIR  := config

# Target executable
TARGET ?= $(BIN_DIR)/game$(EXEEXT)
ifeq ($(HOST_PLATFORM),windows)
RUN_TARGET := $(subst /,\,$(TARGET))
else
RUN_TARGET := ./$(TARGET)
endif

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
	@$(call MKDIR_CMD,$(OBJ_DIR))
	@$(call MKDIR_CMD,$(BIN_DIR))
	@$(call MKDIR_CMD,$(DATA_DIR))
	@$(call MKDIR_CMD,$(CONFIG_DIR))

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS) $(STATIC_RUNTIME)
	@echo "Build successful! Executable is at $(TARGET)"

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(call MKDIR_CMD,$(patsubst %/,%,$(dir $@)))
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the game
run: all
	$(RUN_TARGET)

# Clean up generated files
clean:
	@$(call RM_CMD,$(OBJ_DIR))
	@$(call RM_CMD,$(BIN_DIR))
	@echo "Cleaned up $(OBJ_DIR) and $(BIN_DIR)"

# Rebuild everything from scratch
rebuild: clean all

.PHONY: all clean rebuild run directories
