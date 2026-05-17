CC := clang
CFLAGS := -O2 -Wall -Wextra -Werror -std=c17
DEBUGFLAGS := -g -O0 -Wall -Wextra -Werror -std=c17

SRC_DIR := src
SRC_FILES := $(SRC_DIR)/main.c
OBJ_DIR := obj
OBJ_FILES := $(OBJ_DIR)/main.o
BIN_DIR := bin
TARGET := $(BIN_DIR)/main

DEBUG_OBJ_DIR := debug/obj
DEBUG_OBJ_FILES := $(DEBUG_OBJ_DIR)/main.o
DEBUG_BIN_DIR := debug/bin
DEBUG_TARGET := $(DEBUG_BIN_DIR)/main

.PHONY: all run debug clean

all: $(TARGET)

run: $(TARGET)
	./$<

debug: $(DEBUG_TARGET)
	lldb $<

$(TARGET): $(OBJ_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(DEBUG_TARGET): $(DEBUG_OBJ_FILES)
	mkdir -p $(DEBUG_BIN_DIR)
	$(CC) $(DEBUGFLAGS) -o $@ $^

$(DEBUG_OBJ_FILES): $(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(DEBUG_OBJ_DIR)
	$(CC) -c $(DEBUGFLAGS) -o $@ $<

$(OBJ_FILES): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DEBUG_OBJ_DIR) $(DEBUG_BIN_DIR)
