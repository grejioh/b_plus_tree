CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -std=c99 -pedantic -Wno-unused-parameter -lpthread
TARGET_DIR = target
SRC_DIR = src
INCLUDE_DIR = include
SOURCES = $(wildcard $(SRC_DIR)/*.c)
SOURCES_CLIENT = $(wildcard client/*.c)

$(TARGET_DIR)/server: $(SOURCES)
	mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $^ -o $@

client: $(TARGET_DIR)/client

$(TARGET_DIR)/client: $(SOURCES_CLIENT)
	mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $^ -o $@

clean:
	rm -rf $(TARGET_DIR)