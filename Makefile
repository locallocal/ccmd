# Convenience targets for ccmd. The real build is driven by CMake (see build.sh).

CLANG_FORMAT ?= clang-format
BUILD_SH     := ./build.sh
EXAMPLE_BIN  := build/ccmd-example
ARGS         ?= --help
FORMAT_DIRS  := include example tests
FORMAT_FILES := $(shell find $(FORMAT_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' \) | sort)

.DEFAULT_GOAL := release

.PHONY: help release debug test cov clean example format format-check

help:
	@printf '%s\n' \
		"Targets:" \
		"  release       Configure and build an optimized binary (default)." \
		"  debug         Configure and build with debug symbols." \
		"  test          Configure, build, and run the test suite." \
		"  cov           Run tests and generate an HTML coverage report." \
		"  clean         Remove generated build and coverage directories." \
		"  example       Build and run the example binary with ARGS (default: --help)." \
		"  format        Reformat sources in $(FORMAT_DIRS) with clang-format (in place)." \
		"  format-check  Verify sources are formatted; exit non-zero on differences." \
		"" \
		"Variables:" \
		"  ARGS          Arguments passed to the example binary (e.g. make example ARGS=\"meta --help\")." \
		"  CLANG_FORMAT  clang-format binary to use (default: clang-format)."

release:
	$(BUILD_SH) --release

debug:
	$(BUILD_SH) --debug

test:
	$(BUILD_SH) --test

cov:
	$(BUILD_SH) --cov

clean:
	$(BUILD_SH) --clean

example: release
	$(EXAMPLE_BIN) $(ARGS)

format:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo 'error: $(CLANG_FORMAT) not found' >&2; exit 1; }
	@for f in $(FORMAT_FILES); do \
		echo "$(CLANG_FORMAT) -i --style=file $$f"; \
		$(CLANG_FORMAT) -i --style=file "$$f" || exit 1; \
	done

format-check:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo 'error: $(CLANG_FORMAT) not found' >&2; exit 1; }
	@status=0; \
	for f in $(FORMAT_FILES); do \
		echo "$(CLANG_FORMAT) --dry-run --Werror --style=file $$f"; \
		$(CLANG_FORMAT) --dry-run --Werror --style=file "$$f" || status=1; \
	done; \
	exit $$status
