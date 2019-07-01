CXXFLAGS = -O0 -g  -Wall -I ../.. -pthread
LDFLAGS = -lpthread
BINARIES = test1 test2

SRC = ../thread.cc
all: $(BINARIES)

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(SRC) $(filter %.cc,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) core

test1: test1.cc
test2: test2.cc
