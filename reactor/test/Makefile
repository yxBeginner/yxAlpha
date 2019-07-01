CXXFLAGS = -O0 -g  -Wall -I ../.. -pthread
LDFLAGS = -lpthread
BINARIES = test1.out test2.out test3.out test4.out test5.out test6.out

SRC = ../../thread/thread.cc  ../dispatcher.cc ../eventhandler.cc ../selector.cc ../dispatchthread.cc
all: $(BINARIES)

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(SRC) $(filter %.cc,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) core

test1.out: test1.cc 
test2.out: test2.cc
test3.out: test3.cc
test4.out: test4.cc
test5.out: test5.cc
test6.out: test6.cc
