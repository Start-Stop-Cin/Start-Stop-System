CC      := clang-18
CFLAGS  := -std=c99 -Wall -Wextra -Wpedantic -Werror
INCLUDE := -I include/
SRC_DIR := src
OBJ_DIR := build
OBJ_DIR_MARKER := $(OBJ_DIR)/.dir
BIN     := $(OBJ_DIR)/start_stop_demo

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Testing
UNITY_SRC  := tests/vendor/unity/src
TEST_DIR   := tests
PROD_SRCS  := $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
TEST_SRCS  := $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS  := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/%, $(TEST_SRCS))

COV_DIR    := $(OBJ_DIR)/coverage
COV_FLAGS  := -fprofile-instr-generate -fcoverage-mapping -fcoverage-mcdc -g -O0
COV_BINS   := $(patsubst $(TEST_DIR)/%.c, $(COV_DIR)/%, $(TEST_SRCS))

MUT_DIR    := $(OBJ_DIR)/mutation
MUT_FLAGS  := -g -O0 -fno-omit-frame-pointer \
               -fpass-plugin=/usr/lib/mull-ir-frontend-18 \
               -grecord-command-line
MUT_BINS   := $(patsubst $(TEST_DIR)/%.c, $(MUT_DIR)/%, $(TEST_SRCS))

.PHONY: all build misra format-check clean test coverage mutation

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

# Unity compiled without -Werror to avoid upstream warnings
$(OBJ_DIR)/unity.o: $(UNITY_SRC)/unity.c | $(OBJ_DIR_MARKER)
	$(CC) -std=c99 -I$(UNITY_SRC) -c $< -o $@

$(COV_DIR)/.dir:
	mkdir -p $(COV_DIR) && touch $@

$(COV_DIR)/unity.o: $(UNITY_SRC)/unity.c | $(COV_DIR)/.dir
	$(CC) -std=c99 $(COV_FLAGS) -I$(UNITY_SRC) -c $< -o $@

$(MUT_DIR)/.dir:
	mkdir -p $(MUT_DIR) && touch $@

$(MUT_DIR)/unity.o: $(UNITY_SRC)/unity.c | $(MUT_DIR)/.dir
	$(CC) -std=c99 $(MUT_FLAGS) -I$(UNITY_SRC) -c $< -o $@

# Test binaries
$(OBJ_DIR)/test_%: $(TEST_DIR)/test_%.c $(PROD_SRCS) $(OBJ_DIR)/unity.o | $(OBJ_DIR_MARKER)
	$(CC) $(CFLAGS) -g -O0 $(INCLUDE) -I$(UNITY_SRC) $< $(PROD_SRCS) $(OBJ_DIR)/unity.o -o $@

# Coverage binaries
$(COV_DIR)/test_%: $(TEST_DIR)/test_%.c $(PROD_SRCS) $(COV_DIR)/unity.o | $(COV_DIR)/.dir
	$(CC) $(CFLAGS) $(COV_FLAGS) $(INCLUDE) -I$(UNITY_SRC) $< $(PROD_SRCS) $(COV_DIR)/unity.o -o $@

# Mutation object files (one .c -> one .o so Mull can replay each command)
MUT_PROD_OBJS := $(patsubst $(SRC_DIR)/%.c, $(MUT_DIR)/%.o, $(PROD_SRCS))

$(MUT_DIR)/%.o: $(SRC_DIR)/%.c | $(MUT_DIR)/.dir
	$(CC) $(CFLAGS) $(MUT_FLAGS) $(INCLUDE) -c $< -o $@

# Mutation binaries (link step)
$(MUT_DIR)/test_%: $(TEST_DIR)/test_%.c $(MUT_PROD_OBJS) $(MUT_DIR)/unity.o | $(MUT_DIR)/.dir
	$(CC) $(CFLAGS) $(MUT_FLAGS) $(INCLUDE) -I$(UNITY_SRC) $< $(MUT_PROD_OBJS) $(MUT_DIR)/unity.o -o $@

test: $(TEST_BINS)
	@failed=0; \
	for t in $(TEST_BINS); do echo "--- $$t ---"; $$t || failed=1; done; \
	exit $$failed

coverage: $(COV_BINS)
	@for t in $(COV_BINS); do \
	    LLVM_PROFILE_FILE="$(COV_DIR)/$$(basename $$t).profraw" $$t || true; \
	done
	llvm-profdata-18 merge -sparse $(COV_DIR)/*.profraw -o $(COV_DIR)/merged.profdata
	llvm-cov-18 report $(COV_BINS) \
	    -instr-profile=$(COV_DIR)/merged.profdata \
	    -ignore-filename-regex='(unity|tests)' \
	    --show-branch-summary --show-mcdc-summary
	llvm-cov-18 show $(COV_BINS) \
	    -instr-profile=$(COV_DIR)/merged.profdata \
	    -show-branches=count -show-mcdc \
	    -ignore-filename-regex='(unity|tests)' \
	    -format=html > $(COV_DIR)/coverage.html
	@echo "HTML report: $(COV_DIR)/coverage.html"

mutation: $(MUT_BINS)
	@failed=0; \
	for t in $(MUT_BINS); do \
	    name=$$(basename $$t | sed 's/^test_//'); \
	    run_dir="$(MUT_DIR)/run_$$name"; \
	    mkdir -p $$run_dir; \
	    { \
	        echo "mutators:"; \
	        awk '/^mutators:/{flag=1; next} /^[a-zA-Z]/{flag=0} flag' mull.yml; \
	        echo "includePaths:"; \
	        echo "  - .*/src/$$name\\.c"; \
	    } > $$run_dir/mull.yml; \
	    echo "--- Mutation: $$t (src/$$name.c) ---"; \
	    abs_t=$$(realpath $$t); \
	    (cd $$run_dir && mull-runner-18 $$abs_t) || failed=1; \
	done; \
	exit $$failed
