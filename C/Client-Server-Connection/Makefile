CC := gcc
CFLAGS := -g -Werror -std=c99

BUILD_DIR := ./build
SRC_DIRS := ./src
TARGETS := client server

.PHONY: all clean
all: $(TARGETS:%=$(BUILD_DIR)/%)

# Link executable from object file
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o
	mkdir -p $(dir $@)
	$(CC) $< -o $@

# Compile each .c into object file
$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)
