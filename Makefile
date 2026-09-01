CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -g
LDFLAGS = 

SRC_DIR = src
INC_DIR = include
SCAN_DIR = $(SRC_DIR)/scan
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/peek

SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SCAN_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c | $(SCAN_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean fclean re

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
# 	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
		$(CC) $(CFLAGS) -c $< -o $@

vpath %.c $(SRC_DIR) $(SCAN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -rf $(BIN_DIR)

re: fclean all