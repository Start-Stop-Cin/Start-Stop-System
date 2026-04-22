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
	mkdir -p $(COV_DIR)
	touch $@

$(COV_DIR)/unity.o: $(UNITY_SRC)/unity.c | $(COV_DIR)/.dir
	$(CC) -std=c99 $(COV_FLAGS) -I$(UNITY_SRC) -c $< -o $@

$(MUT_DIR)/.dir:
	mkdir -p $(MUT_DIR)
	touch $@

$(MUT_DIR)/unity.o: $(UNITY_SRC)/unity.c | $(MUT_DIR)/.dir
	$(CC) -std=c99 $(MUT_FLAGS) -I$(UNITY_SRC) -c $< -o $@

# ==========================================================
# UNIT_TEST only for ss_operation
# ==========================================================
$(OBJ_DIR)/test_ss_operation: EXTRA_DEFS := -DUNIT_TEST
$(COV_DIR)/test_ss_operation: EXTRA_DEFS := -DUNIT_TEST
$(MUT_DIR)/test_ss_operation: EXTRA_DEFS := -DUNIT_TEST
$(MUT_DIR)/ss_operation.o: EXTRA_DEFS := -DUNIT_TEST

# Test binaries
$(OBJ_DIR)/test_%: $(TEST_DIR)/test_%.c $(PROD_SRCS) $(OBJ_DIR)/unity.o | $(OBJ_DIR_MARKER)
	$(CC) $(CFLAGS) -g -O0 $(EXTRA_DEFS) $(INCLUDE) -I$(UNITY_SRC) \
		$< $(PROD_SRCS) $(OBJ_DIR)/unity.o -o $@

# Coverage binaries
$(COV_DIR)/test_%: $(TEST_DIR)/test_%.c $(PROD_SRCS) $(COV_DIR)/unity.o | $(COV_DIR)/.dir
	$(CC) $(CFLAGS) $(COV_FLAGS) $(EXTRA_DEFS) $(INCLUDE) -I$(UNITY_SRC) \
		$< $(PROD_SRCS) $(COV_DIR)/unity.o -o $@

# Mutation object files
MUT_PROD_OBJS := $(patsubst $(SRC_DIR)/%.c, $(MUT_DIR)/%.o, $(PROD_SRCS))

$(MUT_DIR)/%.o: $(SRC_DIR)/%.c | $(MUT_DIR)/.dir
	$(CC) $(CFLAGS) $(MUT_FLAGS) $(EXTRA_DEFS) $(INCLUDE) -c $< -o $@

# Mutation binaries
$(MUT_DIR)/test_%: $(TEST_DIR)/test_%.c $(MUT_PROD_OBJS) $(MUT_DIR)/unity.o | $(MUT_DIR)/.dir
	$(CC) $(CFLAGS) $(MUT_FLAGS) $(EXTRA_DEFS) $(INCLUDE) -I$(UNITY_SRC) \
		$< $(MUT_PROD_OBJS) $(MUT_DIR)/unity.o -o $@

# ==========================================================
# Run all tests
# ==========================================================
test: $(TEST_BINS)
	@failed=0; \
	for t in $(TEST_BINS); do \
		echo "--- $$t ---"; \
		$$t || failed=1; \
	done; \
	exit $$failed

# ==========================================================
# Coverage for all tests
# ==========================================================
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
		llvm-profdata-18 merge -sparse "$(COV_DIR)/$$name.profraw" -o "$(COV_DIR)/$$name.profdata"; \
		llvm-cov-18 show $$t \
			-instr-profile="$(COV_DIR)/$$name.profdata" \
			-show-branches=count -show-mcdc \
			-format=html \
			"$(SRC_DIR)/$$name.c" > "$(COV_DIR)/$$name.html"; \
		json=$$(llvm-cov-18 export $$t \
			-instr-profile="$(COV_DIR)/$$name.profdata" \
			-summary-only \
			"$(SRC_DIR)/$$name.c"); \
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
			pct=$$(printf '%.2f' $$percent); \
			if awk "BEGIN {exit !($$percent < $(COV_THRESHOLD))}"; then \
				printf "  %-9s %.2f%% (%s/%s) FAIL (threshold %s%%)\n" "$$metric:" $$percent $$covered $$count $(COV_THRESHOLD); \
				failed=1; \
				mod_failed=1; \
				row="$$row<td class=\"fail\">$$pct%% ($$covered/$$count)</td>"; \
			else \
				printf "  %-9s %.2f%% (%s/%s) OK\n" "$$metric:" $$percent $$covered $$count; \
				row="$$row<td class=\"ok\">$$pct%% ($$covered/$$count)</td>"; \
			fi; \
		done; \
		if [ "$$mod_failed" = "1" ]; then \
			row="$$row<td class=\"fail\"><strong>FAIL</strong></td>"; \
		else \
			row="$$row<td class=\"ok\"><strong>PASS</strong></td>"; \
		fi; \
		echo "$$row</tr>" >> "$(COV_DIR)/rows.html"; \
	done; \
	{ \
		echo "<html><head><meta charset=\"utf-8\"><title>Coverage Summary</title>"; \
		echo "<style>body{font-family:Arial,sans-serif;padding:24px} table{border-collapse:collapse;width:100%} th,td{border:1px solid #ccc;padding:8px;text-align:left} .ok{background:#d4edda} .fail{background:#f8d7da} .na{background:#f2f2f2}</style>"; \
		echo "</head><body>"; \
		echo "<h1>Coverage Summary</h1>"; \
		echo "<p>Threshold: $(COV_THRESHOLD)% for lines, branches and MC/DC</p>"; \
		echo "<table><thead><tr><th>Module</th><th>Lines</th><th>Branches</th><th>MC/DC</th><th>Status</th></tr></thead><tbody>"; \
		cat "$(COV_DIR)/rows.html"; \
		echo "</tbody></table></body></html>"; \
	} > "$(COV_DIR)/index.html"; \
	exit $$failed

# ==========================================================
# Mutation for all tests
# ==========================================================
mutation: $(MUT_BINS)
	@if [ -z "$(MUT_BINS)" ]; then \
		echo "No test binaries found — skipping mutation."; \
		exit 0; \
	fi; \
	failed=0; \
	: > $(MUT_DIR)/rows.html; \
	for t in $(MUT_BINS); do \
		name=$$(basename $$t | sed 's/^test_//'); \
		echo "--- Mutation: $$t ---"; \
		mkdir -p "$(MUT_DIR)/$$name"; \
		mull-runner-18 $$t -test-framework=CustomTest --reporters=IDE --report-dir "$(MUT_DIR)/$$name" || true; \
		summary_json="$(MUT_DIR)/$$name/mull-summary.json"; \
		if [ ! -f "$$summary_json" ]; then \
			echo "No Mull summary for $$name"; \
			failed=1; \
			echo "<tr><td>$$name</td><td class=\"fail\">No report</td><td class=\"fail\"><strong>FAIL</strong></td></tr>" >> "$(MUT_DIR)/rows.html"; \
			continue; \
		fi; \
		killed=$$(jq '.killed' "$$summary_json"); \
		survived=$$(jq '.survived' "$$summary_json"); \
		timeout=$$(jq '.timedOut' "$$summary_json"); \
		total=$$((killed + survived + timeout)); \
		if [ "$$total" -eq 0 ]; then \
			score="0.00"; \
		else \
			score=$$(awk "BEGIN {printf \"%.2f\", 100*$$killed/$$total}"); \
		fi; \
		if awk "BEGIN {exit !($$score < $(MUT_THRESHOLD))}"; then \
			echo "Mutation score $$score% below threshold $(MUT_THRESHOLD)%"; \
			failed=1; \
			echo "<tr><td>$$name</td><td class=\"fail\">$$score% (killed=$$killed, survived=$$survived, timeout=$$timeout)</td><td class=\"fail\"><strong>FAIL</strong></td></tr>" >> "$(MUT_DIR)/rows.html"; \
		else \
			echo "Mutation score $$score%"; \
			echo "<tr><td>$$name</td><td class=\"ok\">$$score% (killed=$$killed, survived=$$survived, timeout=$$timeout)</td><td class=\"ok\"><strong>PASS</strong></td></tr>" >> "$(MUT_DIR)/rows.html"; \
		fi; \
	done; \
	{ \
		echo "<html><head><meta charset=\"utf-8\"><title>Mutation Summary</title>"; \
		echo "<style>body{font-family:Arial,sans-serif;padding:24px} table{border-collapse:collapse;width:100%} th,td{border:1px solid #ccc;padding:8px;text-align:left} .ok{background:#d4edda} .fail{background:#f8d7da}</style>"; \
		echo "</head><body>"; \
		echo "<h1>Mutation Summary</h1>"; \
		echo "<p>Threshold: $(MUT_THRESHOLD)%</p>"; \
		echo "<table><thead><tr><th>Module</th><th>Mutation Score</th><th>Status</th></tr></thead><tbody>"; \
		cat "$(MUT_DIR)/rows.html"; \
		echo "</tbody></table></body></html>"; \
	} > "$(MUT_DIR)/index.html"; \
	exit $$failed

# ==========================================================
# Special aliases for operation only
# ==========================================================
test-operation-special: $(OBJ_DIR)/test_ss_operation
	$(OBJ_DIR)/test_ss_operation

coverage-operation-special: $(COV_DIR)/test_ss_operation
	@name=ss_operation; \
	echo "--- Coverage: $(COV_DIR)/test_ss_operation (src/$$name.c) ---"; \
	LLVM_PROFILE_FILE="$(COV_DIR)/$$name.profraw" $(COV_DIR)/test_ss_operation || true; \
	llvm-profdata-18 merge -sparse "$(COV_DIR)/$$name.profraw" -o "$(COV_DIR)/$$name.profdata"; \
	llvm-cov-18 show "$(COV_DIR)/test_ss_operation" \
		-instr-profile="$(COV_DIR)/$$name.profdata" \
		-show-branches=count -show-mcdc \
		-format=html \
		"$(SRC_DIR)/$$name.c" > "$(COV_DIR)/$$name.html"; \
	echo "Generated $(COV_DIR)/$$name.html"

mutation-operation-special: $(MUT_DIR)/test_ss_operation
	@mkdir -p "$(MUT_DIR)/ss_operation"; \
	mull-runner-18 "$(MUT_DIR)/test_ss_operation" -test-framework=CustomTest --reporters=IDE --report-dir "$(MUT_DIR)/ss_operation"
	