CC ?= gcc
CXX ?= g++
AR ?= ar
CLANG_FORMAT ?= clang-format

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
DIST_DIR := dist

STATIC_LIB := $(DIST_DIR)/libEasyAACEncoder.a
SHARED_LIB := $(DIST_DIR)/libEasyAACEncoder.so
SAMPLE_BIN := $(DIST_DIR)/testEasyAACEncoder

INCLUDES := -IInclude -Isrc -Isrc/include -Isrc/libfaac -Isrc/libfaac/kiss_fft
CPPFLAGS += $(INCLUDES)
CXXFLAGS ?= -O2
CXXFLAGS += -std=c++23 -Wall -Wextra -fPIC
CFLAGS ?= -O2
CFLAGS += -std=gnu99 -fPIC
LDFLAGS +=
LDLIBS += -lm -lpthread

OWNED_CPP_SRCS := \
	src/EasyAACEncoderAPI.cpp \
	src/EasyAACEncoder.cpp \
	src/IDecodeToPcm.cpp \
	src/G711AToPcm.cpp \
	src/G726ToPcm.cpp \
	src/PcmToAac.cpp \
	src/audio_buffer.cpp \
	src/EasyDSSBuffers.cpp \
	src/g711.cpp \
	src/g726.cpp

VENDORED_C_SRCS := \
	src/libfaac/aacquant.c \
	src/libfaac/backpred.c \
	src/libfaac/bitstream.c \
	src/libfaac/channels.c \
	src/libfaac/fft.c \
	src/libfaac/filtbank.c \
	src/libfaac/frame.c \
	src/libfaac/huffman.c \
	src/libfaac/ltp.c \
	src/libfaac/midside.c \
	src/libfaac/psychkni.c \
	src/libfaac/tns.c \
	src/libfaac/util.c \
	src/libfaac/kiss_fft/kiss_fft.c \
	src/libfaac/kiss_fft/kiss_fftr.c

LIB_OBJS := \
	$(OWNED_CPP_SRCS:%.cpp=$(OBJ_DIR)/%.o) \
	$(VENDORED_C_SRCS:%.c=$(OBJ_DIR)/%.o)

.PHONY: all static shared sample warnings fmt clean help

all: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

sample: $(SAMPLE_BIN)

warnings:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Werror -c $(OWNED_CPP_SRCS)
	rm -f $(notdir $(OWNED_CPP_SRCS:.cpp=.o))

fmt:
	git ls-files '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hpp' '*.hh' | xargs -r $(CLANG_FORMAT) -i

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)
	rm -f src/testEasyAACEncoder/*.aac testEasyAACEncoder/*.aac

help:
	@echo "Available targets:"
	@echo "  make          Build static and shared EasyAACEncoder libraries"
	@echo "  make static   Build dist/libEasyAACEncoder.a"
	@echo "  make shared   Build dist/libEasyAACEncoder.so"
	@echo "  make sample   Build dist/testEasyAACEncoder"
	@echo "  make warnings Compile project-owned C++23 sources with -Wall -Wextra -Werror"
	@echo "  make fmt      Format tracked C/C++ files with clang-format"
	@echo "  make clean    Remove generated build artifacts"

$(STATIC_LIB): $(LIB_OBJS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

$(SHARED_LIB): $(LIB_OBJS)
	@mkdir -p $(@D)
	$(CXX) -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(SAMPLE_BIN): testEasyAACEncoder/main.cpp $(STATIC_LIB)
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(STATIC_LIB) $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
