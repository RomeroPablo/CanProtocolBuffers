CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
LDFLAGS ?=
LDLIBS ?= -lcapnp -lkj

SCHEMA := schema/addressbook.capnp
GEN_CPP := src/schema/addressbook.capnp.c++
GEN_H := src/schema/addressbook.capnp.h
APP := addressbook-demo

all: run

gen: $(GEN_CPP) $(GEN_H)

$(GEN_CPP) $(GEN_H): $(SCHEMA)
	capnp compile -oc++:src -Ischema $(SCHEMA)

$(APP): src/main.c++ gen
	$(CXX) $(CXXFLAGS) src/main.c++ $(GEN_CPP) -o $(APP) $(LDFLAGS) $(LDLIBS)

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP) addressbook.bin $(GEN_CPP) $(GEN_H)

.PHONY: all gen run clean
