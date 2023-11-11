CC = gcc
CFLAGS = -g
TARGET_DIR = target
SRC_DIR = src
INCLUDE_DIR = include

$(TARGET_DIR)/main: $(SRC_DIR)/main.c $(SRC_DIR)/B_plus_tree.c
	mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $^ -o $@

clean:
	rm -rf $(TARGET_DIR)