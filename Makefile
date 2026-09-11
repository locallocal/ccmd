# Convenience targets for ccmd. The real build is driven by CMake (see build.sh).

CLANG_FORMAT ?= clang-format
FORMAT_DIRS  := include example tests
FORMAT_FILES := $(shell find $(FORMAT_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' \) | sort)

.PHONY: format format-check help

help:
	@printf '%s\n' \
		"Targets:" \
		"  format        Reformat sources in $(FORMAT_DIRS) with clang-format (in place)." \
		"  format-check  Verify sources are formatted; exit non-zero on differences." \
		"" \
		"Variables:" \
		"  CLANG_FORMAT  clang-format binary to use (default: clang-format)."

format:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo 'error: $(CLANG_FORMAT) not found' >&2; exit 1; }
	$(CLANG_FORMAT) -i --style=file $(FORMAT_FILES)

format-check:
	@command -v $(CLANG_FORMAT) >/dev/null 2>&1 || { echo 'error: $(CLANG_FORMAT) not found' >&2; exit 1; }
	$(CLANG_FORMAT) --dry-run --Werror --style=file $(FORMAT_FILES)
