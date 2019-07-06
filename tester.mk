CXXFLAGS = -O0 -g  -Wall -I ../.. -pthread
LDFLAGS = -lpthread -lyxalp -L.
BASE_SRC = logger/logstream.cc logger/asynclogging.cc logger/fileutil.cc logger/logfile.cc \
logger/logging.cc thread/thread.cc threadpool.cc
SRCS = $(notdir $(LIB_SRC) $(BASE_SRC))
OBJS = $(patsubst %.cc,%.o,$(SRCS))

libyxalp.a: $(BASE_SRC) $(LIB_SRC)
	g++ $(CXXFLAGS) -c $^
	ar rcs $@ $(OBJS)

$(BINARIES): libyxalp.a
	g++ $(CXXFLAGS) -o $@ $(filter %.cc,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) *.o *.a core

