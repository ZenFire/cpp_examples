
################################################################################
################################################################################

CXX=g++ -g

BIN=account_watcher market_watcher_handler place_order
OBJS=account_watcher.o market_watcher_handler.o place_order.o
TOP_DIR=../..

ifdef USESHARED
	LIBS=$(TOP_DIR)/lib*/libzenfire.so -lpthread -lssl -lcrypto -lrt
else
	LIBS=$(TOP_DIR)/lib*/libzenfire.a -lpthread -lssl -lcrypto
endif

.PHONY: all clean test
all : $(BIN)

clean:
	-$(RM) $(OBJS) $(BIN)

test:
	@echo no tests

CPPFLAGS+= -I$(TOP_DIR)/include -I .. -std=c++0x
LDFLAGS+= -L$(TOP_DIR)/lib64

ifdef USE32
	CPPFLAGS += -m32
	LDFLAGS += -m32
endif 

################################################################################


account_watcher.o: ../account_watcher/account_watcher.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

market_watcher_handler.o: ../market_watcher_handler/market_watcher_handler.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

place_order.o: ../place_order/place_order.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

account_watcher: account_watcher.o
	$(CXX) $(LDFLAGS) account_watcher.o -o "$@" $(LIBS)

market_watcher_handler: market_watcher_handler.o
	$(CXX) $(LDFLAGS) market_watcher_handler.o -o "$@" $(LIBS)

place_order: place_order.o
	$(CXX) $(LDFLAGS) place_order.o -o "$@" $(LIBS)

################################################################################
################################################################################

