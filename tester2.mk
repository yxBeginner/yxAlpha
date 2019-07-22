CXXFLAGS = -O0 -g  -Wall -I ../.. -pthread
LDFLAGS = -lpthread
BASE_SRC = ../../logger/logstream.cc ../../logger/asynclogging.cc ../../logger/fileutil.cc ../../logger/logfile.cc \
../../logger/logging.cc ../../thread/thread.cc ../../thread/threadpool.cc \
../../reactor/dispatcher.cc ../../reactor/dispatchthread.cc ../../reactor/eventhandler.cc  ../../reactor/selector.cc \
../../utility/buffer.cc
../../http/httpcontext.cc ../../http/httpresponse.cc ../../http/httpserver.cc

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(LIB_SRC) $(BASE_SRC) $(filter %.cc,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) core

