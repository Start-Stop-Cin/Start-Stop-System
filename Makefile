CC      := clang-18
CFLAGS  := -std=c99 -Wall -Wextra -Wpedantic -Werror
INCLUDE := -I include/
SRC_DIR := src
OBJ_DIR := build
OBJ_DIR_MARKER := $(OBJ_DIR)/.dir
BIN     := $(OBJ_DIR)/start_stop_demo

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all build misra format-check clean

all: build

# Compile all sources and link the final binary
build: $(BIN)

$(OBJ_DIR_MARKER):
	mkdir -p $(OBJ_DIR)
	touch $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR_MARKER)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Run MISRA C 2012 static analysis and save the report to build/
misra: | $(OBJ_DIR_MARKER)
	cppcheck \
		--std=c99 \
		--addon=misra \
		--error-exitcode=1 \
		--enable=warning,style,performance,portability \
		--suppress=missingIncludeSystem \
		-I include/ \
		--xml --xml-version=2 \
		src/*.c 2>$(OBJ_DIR)/cppcheck-report.xml

# Check formatting without modifying files
format-check:
	clang-format -style=file --dry-run --Werror $(wildcard $(SRC_DIR)/*.c) $(wildcard include/*.h)

# Remove all build artifacts
clean:
	rm -rf $(OBJ_DIR)
