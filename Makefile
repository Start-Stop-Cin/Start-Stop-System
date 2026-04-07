CC      := clang-18
CFLAGS  := -std=c99 -Wall -Wextra -Wpedantic -Werror
INCLUDE := -I include/
SRC_DIR := src
OBJ_DIR := build
BIN     := $(OBJ_DIR)/start_stop_obj

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all build misra format-check clean

all: build

# Compile all sources and link the final binary
build: $(BIN)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Run MISRA C 2012 static analysis and save the report to build/
misra: | $(OBJ_DIR)
	cppcheck \
		--std=c99 \
		--addon=misra \
		--error-exitcode=1 \
		--enable=warning,style,performance,portability \
		--suppress=missingIncludeSystem \
		-I include/ \
		--xml --xml-version=2 \
		src/ 2>$(OBJ_DIR)/cppcheck-report.xml

# Check formatting without modifying files
format-check:
	clang-format --dry-run --Werror $(wildcard $(SRC_DIR)/*.c) $(wildcard include/*.h)

# Remove all build artifacts
clean:
	rm -rf $(OBJ_DIR)
