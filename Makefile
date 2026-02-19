CXX ?= g++
CXXFLAGS ?= -std=c++23 -O2 -Wall -Wextra -pedantic
THIRD_PARTY_CXXFLAGS ?= -std=c++23 -O2 -w
CPPFLAGS ?= -Isrc/lib -Isrc
LDFLAGS ?=
LDLIBS ?= -pthread

SCHEMA := schema/can.capnp
ARTIFACTS_DIR := artifacts
CC_JSON := compile_commands.json
BUILD_DIR := $(ARTIFACTS_DIR)/obj
GEN_CPP := src/schema/can.capnp.c++
GEN_H := src/schema/can.capnp.h
APP := $(ARTIFACTS_DIR)/can-demo

CAPNP_SRCS := \
	src/lib/capnp/message.c++ \
	src/lib/capnp/arena.c++ \
	src/lib/capnp/layout.c++ \
	src/lib/capnp/list.c++ \
	src/lib/capnp/serialize.c++ \
	src/lib/capnp/serialize-packed.c++ \
	src/lib/capnp/blob.c++

KJ_SRCS := \
	src/lib/kj/common.c++ \
	src/lib/kj/array.c++ \
	src/lib/kj/debug.c++ \
	src/lib/kj/exception.c++ \
	src/lib/kj/io.c++ \
	src/lib/kj/memory.c++ \
	src/lib/kj/string.c++ \
	src/lib/kj/mutex.c++ \
	src/lib/kj/hash.c++ \
	src/lib/kj/table.c++ \
	src/lib/kj/units.c++

MAIN_OBJ := $(BUILD_DIR)/main.o
SCHEMA_OBJ := $(BUILD_DIR)/schema/can.capnp.o
CAPNP_OBJS := $(patsubst src/lib/%.c++,$(BUILD_DIR)/lib/%.o,$(CAPNP_SRCS))
KJ_OBJS := $(patsubst src/lib/%.c++,$(BUILD_DIR)/lib/%.o,$(KJ_SRCS))
OBJS := $(MAIN_OBJ) $(SCHEMA_OBJ) $(CAPNP_OBJS) $(KJ_OBJS)

all: run

$(APP): $(OBJS) | $(ARTIFACTS_DIR)
	$(CXX) $(OBJS) -o $(APP) $(LDFLAGS) $(LDLIBS)

$(MAIN_OBJ): src/main.cpp | $(ARTIFACTS_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(SCHEMA_OBJ): $(GEN_CPP) $(GEN_H) | $(ARTIFACTS_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(GEN_CPP) -o $@

$(BUILD_DIR)/lib/%.o: src/lib/%.c++ | $(ARTIFACTS_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(THIRD_PARTY_CXXFLAGS) -c $< -o $@

run: $(APP)
	$(APP)

$(ARTIFACTS_DIR):
	mkdir -p $(ARTIFACTS_DIR)

regen:
	capnp compile -oc++:src/ -Ischema $(SCHEMA)

clean:
	rm -rf $(ARTIFACTS_DIR) $(CC_JSON)

.PHONY: all run clean regen
