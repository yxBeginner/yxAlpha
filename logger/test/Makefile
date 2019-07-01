CXXFLAGS = -O0 -g  -Wall -I ../.. -pthread
LDFLAGS = -lpthread
BINARIES = test1.out test5.out

SRC = ../logstream.cc ../asynclogging.cc ../fileutil.cc ../logfile.cc ../logging.cc ../../thread/thread.cc
all: $(BINARIES)

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(SRC) $(filter %.cc,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) core

test1.out: test1.cc 
test5.out: test5.cc
