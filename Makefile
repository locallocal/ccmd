# Convenience targets for ccmd. The real build is driven by CMake (see build.sh).

CLANG_FORMAT ?= clang-format
BUILD_SH     := ./build.sh
FORMAT_DIRS  := include example tests
FORMAT_FILES := $(shell find $(FORMAT_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' \) | sort)

.DEFAULT_GOAL := release

.PHONY: help release debug test cov clean format format-check

help:
	@printf '%s\n' \
		"Targets:" \
		"  release       Configure and build an optimized binary (default)." \
		"  debug         Configure and build with debug symbols." \
		"  test          Configure, build, and run the test suite." \
		"  cov           Run tests and generate an HTML coverage report." \
		"  clean         Remove generated build and coverage directories." \
		"  format        Reformat sources in $(FORMAT_DIRS) with clang-format (in place)." \
		"  format-check  Verify sources are formatted; exit non-zero on differences." \
		"" \
		"Variables:" \
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

format:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo 'error: $(CLANG_FORMAT) not found' >&2; exit 1; }
	$(CLANG_FORMAT) -i --style=file $(FORMAT_FILES)

format-check:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo 'error: $(CLANG_FORMAT) not found' >&2; exit 1; }
	$(CLANG_FORMAT) --dry-run --Werror --style=file $(FORMAT_FILES)
