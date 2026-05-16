CC := clang
CFLAGS := -O2 -Wall -Wextra -Werror -std=c17

SRC_DIR := src
SRC_FILES := $(SRC_DIR)/main.c
OBJ_DIR := obj
OBJ_FILES := $(OBJ_DIR)/main.o
BIN_DIR := bin
TARGET := $(BIN_DIR)/main

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_FILES): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
