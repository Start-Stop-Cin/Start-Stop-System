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
COV_THRESHOLD ?= 80

MUT_DIR    := $(OBJ_DIR)/mutation
MUT_FLAGS  := -g -O0 -fno-omit-frame-pointer \
              -fpass-plugin=/usr/lib/mull-ir-frontend-18 \
              -grecord-command-line
MUT_BINS   := $(patsubst $(TEST_DIR)/%.c, $(MUT_DIR)/%, $(TEST_SRCS))
MUT_THRESHOLD ?= 80

.PHONY: all build misra format-check clean \
        test coverage mutation \
        test-operation-special coverage-operation-special mutation-operation-special

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

# ==========================================================
# UNIT_TEST only for ss_operation
# Minimal change: only operation gets wrappers enabled
# ==========================================================
$(OBJ_DIR)/test_ss_operation: EXTRA_DEFS := -DUNIT_TEST
$(COV_DIR)/test_ss_operation: EXTRA_DEFS := -DUNIT_TEST
$(MUT_DIR)/test_ss_operation: EXTRA_DEFS := -DUNIT_TEST
$(MUT_DIR)/ss_operation.o: EXTRA_DEFS := -DUNIT_TEST

# Test binaries
$(OBJ_DIR)/test_%: $(TEST_DIR)/test_%.c $(PROD_SRCS) $(OBJ_DIR)/unity.o | $(OBJ_DIR_MARKER)
	$(CC) $(CFLAGS) -g -O0 $(EXTRA_DEFS) $(INCLUDE) -I$(UNITY_SRC) $< $(PROD_SRCS) $(OBJ_DIR)/unity.o -o $@

# Coverage binaries
$(COV_DIR)/test_%: $(TEST_DIR)/test_%.c $(PROD_SRCS) $(COV_DIR)/unity.o | $(COV_DIR)/.dir
	$(CC) $(CFLAGS) $(COV_FLAGS) $(EXTRA_DEFS) $(INCLUDE) -I$(UNITY_SRC) $< $(PROD_SRCS) $(COV_DIR)/unity.o -o $@

# Mutation object files (one .c -> one .o so Mull can replay each command)
MUT_PROD_OBJS := $(patsubst $(SRC_DIR)/%.c, $(MUT_DIR)/%.o, $(PROD_SRCS))

$(MUT_DIR)/%.o: $(SRC_DIR)/%.c | $(MUT_DIR)/.dir
	$(CC) $(CFLAGS) $(MUT_FLAGS) $(EXTRA_DEFS) $(INCLUDE) -c $< -o $@

# Mutation binaries (link step)
$(MUT_DIR)/test_%: $(TEST_DIR)/test_%.c $(MUT_PROD_OBJS) $(MUT_DIR)/unity.o | $(MUT_DIR)/.dir
	$(CC) $(CFLAGS) $(MUT_FLAGS) $(EXTRA_DEFS) $(INCLUDE) -I$(UNITY_SRC) $< $(MUT_PROD_OBJS) $(MUT_DIR)/unity.o -o $@

test: $(TEST_BINS)
	@failed=0; \
	for t in $(TEST_BINS); do echo "--- $$t ---"; $$t || failed=1; done; \
	exit $$failed

coverage: $(COV_BINS)
	@if [ -z "$(COV_BINS)" ]; then \
	    echo "No test binaries found — skipping coverage."; \
	    exit 0; \
	fi; \
	failed=0; \
	: > $(COV_DIR)/rows.html; \
	for t in $(COV_BINS); do \
	    name=$$(basename $$t | sed 's/^test_//'); \
	    echo "--- Coverage: $$t (src/$$name.c) ---"; \
	    LLVM_PROFILE_FILE="$(COV_DIR)/$$name.profraw" $$t || true; \
	    llvm-profdata-18 merge -sparse $(COV_DIR)/$$name.profraw -o $(COV_DIR)/$$name.profdata; \
	    llvm-cov-18 show $$t \
	        -instr-profile=$(COV_DIR)/$$name.profdata \
	        -show-branches=count -show-mcdc \
	        -format=html \
	        $(SRC_DIR)/$$name.c > $(COV_DIR)/$$name.html; \
	    json=$$(llvm-cov-18 export $$t \
	        -instr-profile=$(COV_DIR)/$$name.profdata \
	        -summary-only \
	        $(SRC_DIR)/$$name.c); \
	    row="<tr><td><a href=\"$$name.html\">$$name</a></td>"; \
	    mod_failed=0; \
	    for metric in lines branches mcdc; do \
	        count=$$(echo "$$json" | jq ".data[0].totals.$$metric.count"); \
	        covered=$$(echo "$$json" | jq ".data[0].totals.$$metric.covered"); \
	        percent=$$(echo "$$json" | jq ".data[0].totals.$$metric.percent"); \
	        if [ "$$count" = "0" ]; then \
	            printf "  %-9s N/A (no instances)\n" "$$metric:"; \
	            row="$$row<td class=\"na\">N/A</td>"; \
	            continue; \
	        fi; \
	        pct=$$(printf '%.2f' $$percent | sed 's/%//g'); \
	        if awk "BEGIN {exit !($$percent < $(COV_THRESHOLD))}"; then \
	            printf "  %-9s %.2f%% (%s/%s) FAIL (threshold %s%%)\n" "$$metric:" $$percent $$covered $$count $(COV_THRESHOLD); \
	            failed=1; \
	            mod_failed=1; \
	            row="$$row<td class=\"fail\">$$pct% ($$covered/$$count)</td>"; \
	        else \
	            printf "  %-9s %.2f%% (%s/%s) OK\n" "$$metric:" $$percent $$covered $$count; \
	            row="$$row<td class=\"ok\">$$pct% ($$covered/$$count)</td>"; \
	        fi; \
	    done; \
	    if [ "$$mod_failed" = "1" ]; then \
	        row="$$row<td class=\"fail\"><strong>FAIL</strong></td>"; \
	    else \
	        row="$$row<td class=\"ok\"><strong>PASS</strong></td>"; \
	    fi; \
	    echo "$$row</tr>" >> $(COV_DIR)/rows.html; \
	    echo "  HTML: $(COV_DIR)/$$name.html"; \
	done; \
	echo ""; \
	echo "--- Combined report ---"; \
	{ \
	    echo '<!DOCTYPE html>'; \
	    echo '<html lang="en"><head><meta charset="utf-8"><title>Coverage Summary</title>'; \
	    echo '<style>'; \
	    echo 'body{font-family:sans-serif;max-width:900px;margin:2em auto;padding:0 1em;color:#222}'; \
	    echo 'h1{margin-bottom:.2em}'; \
	    echo 'table{border-collapse:collapse;width:100%;margin-top:1em}'; \
	    echo 'th,td{border:1px solid #ddd;padding:.5em .8em;text-align:left}'; \
	    echo 'th{background:#f4f4f4}'; \
	    echo 'tr:nth-child(even) td{background:#fafafa}'; \
	    echo '.ok{color:#080;background:#efffef}'; \
	    echo '.fail{color:#b00;background:#ffe8e8}'; \
	    echo '.na{color:#888;background:#f4f4f4}'; \
	    echo '.banner{padding:.8em 1em;border-radius:4px;margin:1em 0;font-weight:bold}'; \
	    echo '.banner.pass{background:#efffef;color:#080;border:1px solid #8c8}'; \
	    echo '.banner.fail{background:#ffe8e8;color:#b00;border:1px solid #e88}'; \
	    echo 'a{color:#06c;text-decoration:none}a:hover{text-decoration:underline}'; \
	    echo '</style></head><body>'; \
	    echo '<h1>Coverage Summary</h1>'; \
	    if [ "$$failed" = "1" ]; then \
	        echo "<div class=\"banner fail\">FAIL — one or more modules below threshold ($(COV_THRESHOLD)%)</div>"; \
	    else \
	        echo "<div class=\"banner pass\">PASS — all modules meet threshold ($(COV_THRESHOLD)%)</div>"; \
	    fi; \
	    echo '<table><thead><tr><th>Module</th><th>Statement</th><th>Branch</th><th>MC/DC</th><th>Status</th></tr></thead><tbody>'; \
	    cat $(COV_DIR)/rows.html; \
	    echo '</tbody></table></body></html>'; \
	} > $(COV_DIR)/index.html; \
	rm -f $(COV_DIR)/rows.html; \
	echo "Combined report: $(COV_DIR)/index.html"; \
	exit $$failed

mutation: $(MUT_BINS)
	@if [ -z "$(MUT_BINS)" ]; then \
	    echo "No test binaries found — skipping mutation testing."; \
	    exit 0; \
	fi; \
	failed=0; \
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
	    (cd $$run_dir && mull-runner-18 --mutation-score-threshold=$(MUT_THRESHOLD) $$abs_t) || failed=1; \
	done; \
	echo ""; \
	if [ "$$failed" = "1" ]; then \
	    echo "FAIL — one or more modules below threshold ($(MUT_THRESHOLD)%)"; \
	else \
	    echo "PASS — all modules meet threshold ($(MUT_THRESHOLD)%)"; \
	fi; \
	exit $$failed
