CXX ?= g++
CXXFLAGS ?= -std=c++23 -O2 -Wall -Wextra -pedantic
LDFLAGS ?=
LDLIBS ?= -lcapnp -lkj

SCHEMA := schema/can.capnp
ARTIFACTS_DIR := artifacts
GEN_CPP := src/schema/can.capnp.c++
GEN_H := src/schema/can.capnp.h
APP := $(ARTIFACTS_DIR)/can-demo

all: run

gen: $(GEN_CPP) $(GEN_H)

$(GEN_CPP) $(GEN_H): $(SCHEMA)
	capnp compile -oc++:src/ -Ischema $(SCHEMA)

$(APP): src/main.cpp gen | $(ARTIFACTS_DIR)
	$(CXX) $(CXXFLAGS) src/main.cpp $(GEN_CPP) -o $(APP) $(LDFLAGS) $(LDLIBS)

run: $(APP) $(ARTIFACTS_DIR)
	$(APP)

$(ARTIFACTS_DIR):
	mkdir -p $(ARTIFACTS_DIR)

clean:
	rm -rf $(ARTIFACTS_DIR) $(GEN_CPP) $(GEN_H)

.PHONY: all gen run clean
