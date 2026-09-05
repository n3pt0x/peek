CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -I$(SRC_DIR)
LDFLAGS = 

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/peek

SRCS = $(shell find $(SRC_DIR) -name "*.c")
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
# OBJS = $(patsubst $(SRC_DIR)/%.c | $(NET_DIR)/%.c | $(PROTOCOLS_DIR)/%.c | $(SCANNER_DIR)/%.c | $(UTILS_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean fclean re

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# $(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
# 		$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
		@mkdir -p $(dir $@)
		$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -rf $(BIN_DIR)

re: fclean all