CC = gcc
CFLAGS = -g
TARGET_DIR = target
SRC_DIR = src

$(TARGET_DIR)/main: $(SRC_DIR)/main.c
	mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGET_DIR)