CXXFLAGS = -O0 -g  -Wall -I ../.. -pthread
LDFLAGS = -lpthread
BINARIES = test_net_server test_net_client

LIB_SRC = ../net/inetaddr.cc ../net/socket.cc
all: $(BINARIES)

$(BINARIES):
	g++ $(CXXFLAGS) -o $@ $(LIB_SRC) $(filter %.cc,$^) $(LDFLAGS)

clean:
	rm -f $(BINARIES) core

test_net_server: test_net_server.cc
test_net_client: test_net_client.cc
