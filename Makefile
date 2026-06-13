CLANG_FORMAT ?= clang-format

CLEAN_DIRS := \
	build \
	dist \
	src/build \
	src/dist \
	src/i386 \
	src/x64 \
	src/x64_shared \
	src/arm \
	src/Debug \
	src/testEasyAACEncoder/build \
	src/testEasyAACEncoder/dist \
	testEasyAACEncoder/build \
	testEasyAACEncoder/dist

CLEAN_FILES := \
	src/testEasyAACEncoder/*.aac \
	src/testEasyAACEncoder/a.out \
	testEasyAACEncoder/*.aac

.PHONY: help fmt clean

help:
	@echo "Available targets:"
	@echo "  fmt    Format tracked C/C++ source files with clang-format"
	@echo "  clean  Remove build artifacts and generated encoder outputs"

fmt:
	git ls-files '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hpp' '*.hh' | xargs -r $(CLANG_FORMAT) -i

clean:
	@if [ -f src/Buildit ]; then (cd src && sh ./Buildit clean); fi
	@if [ -f src/testEasyAACEncoder/nbproject/Makefile-impl.mk ]; then $(MAKE) -C src/testEasyAACEncoder clean; fi
	@if [ -f testEasyAACEncoder/nbproject/Makefile-impl.mk ]; then $(MAKE) -C testEasyAACEncoder clean; fi
	rm -rf $(CLEAN_DIRS)
	rm -f $(CLEAN_FILES)
