CLANG_FORMAT ?= clang-format

.PHONY: help fmt

help:
	@echo "Available targets:"
	@echo "  fmt    Format tracked C/C++ source files with clang-format"

fmt:
	git ls-files '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hpp' '*.hh' | xargs -r $(CLANG_FORMAT) -i
