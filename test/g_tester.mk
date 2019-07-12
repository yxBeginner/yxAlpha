CXXFLAGS = -O0 -g  -Wall -I .. -pthread
LDFLAGS = -lpthread
BASE_SRC = ../logger/logstream.cc ../logger/asynclogging.cc ../logger/fileutil.cc ../logger/logfile.cc \
../logger/logging.cc ../thread/thread.cc ../thread/threadpool.cc \
../reactor/dispatcher.cc ../reactor/dispatchthread.cc ../reactor/eventhandler.cc  ../reactor/selector.cc \
../net/acceptor.cc ../net/inetaddr.cc ../net/socket.cc ../net/tcpconnection.cc ../net/tcpserver.cc \
../utility/buffer.cc

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(BASE_SRC) $(filter %.cc,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) core

