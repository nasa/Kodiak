# System-aware Makefile

arch=$(shell uname -s)

ifeq ($(arch),Darwin)
	myarch = macos
else
ifeq ($(arch),Linux)
	myarch = linux
else
	myarch = $(arch)
endif
endif

KODIAK_DIR = ..
KODIAK_LIB = $(KODIAK_DIR)/lib/$(myarch)

CC = g++ 
CFLAGS = -Wall -frounding-math -pedantic -Wextra -Wno-parentheses -g -O0 -DHAVE_BOOST -I/usr/local/include -I$(BOOST_ROOT)/include -I$(KODIAK_DIR)/src

LDFLAGS = -L/usr/local/lib -L$(BOOST_ROOT)/lib -lprim -lboost_serialization-mt 

all: minmax paving bifurcation

minmax: lib
	$(CC) $(CFLAGS) minmax.cpp $(KODIAK_LIB)/libKodiak.a $(LDFLAGS) -o minmax
	@echo "**"
	@echo "** minmax binary built"
	@echo "**" 

paving: lib
	$(CC) $(CFLAGS) paving.cpp $(KODIAK_LIB)/libKodiak.a $(LDFLAGS) -o paving
	@echo "**"
	@echo "** paving binary built"
	@echo "**" 

bifurcation: lib
	$(CC) $(CFLAGS) bifurcation.cpp $(KODIAK_LIB)/libKodiak.a $(LDFLAGS) -o bifurcation
	@echo "**"
	@echo "** bifurcation binary built"
	@echo "**" 

clean:
	-rm -f .depend *.o minmax paving bifurcation
	-rm -rf minmax.dSYM paving.dSYM bifurcation.dSYM
	-rm callgrind.out*
	-rm *.dat? *.dat *.gplot *.png *.pdf *.pav *out*

lib: 
	cd $(KODIAK_DIR)/src; make

.PHONY: all lib clean minmax paving bifurcation
