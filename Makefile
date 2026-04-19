CC        = gcc
EXEC      = build/portugol.out
SRC_DIR   = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c) \
          $(wildcard $(SRC_DIR)/helpers/*.c) \
          $(wildcard $(SRC_DIR)/diagnostics/*.c) \
          $(wildcard $(SRC_DIR)/debugger/*.c) \
          $(wildcard $(SRC_DIR)/preprocessor/*.c) \
		  $(wildcard $(SRC_DIR)/codegen/*.c)


OBJECTS   = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

CFLAGS    = -g -Wall -Wextra -I$(SRC_DIR)/include

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(EXEC)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

install: $(EXEC)
	cp $(EXEC) /usr/local/bin/portugol

clean:
	-rm -rf $(BUILD_DIR)

.PHONY: install clean
