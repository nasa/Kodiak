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

KODIAK_LIB =../lib/$(myarch)

C_FILES := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(C_FILES))
CC = g++ 
CFLAGS = -Wall -frounding-math -pedantic -Wextra -Wno-parentheses -g -O0 -DHAVE_BOOST -I/usr/local/include -I$(BOOST_ROOT)/include -fPIC

all: build

build: .depend $(OBJS)
	@test -d $(KODIAK_LIB) || mkdir -p $(KODIAK_LIB)
	ar -cr $(KODIAK_LIB)/libKodiak.a $(OBJS) && ranlib $(KODIAK_LIB)/libKodiak.a 
	@echo "**"
	@echo "** Kodiak: C++ Library built in directory lib/$(myarch)/libKodiak.a"
	@echo "**"

depend: .depend

.depend: cmd = gcc -MM -MF depend $(var) -fPIC; cat depend >> .depend;
.depend:
	@$(foreach var, $(C_FILES), $(cmd))
	@rm -f depend

-include .depend

# These are the pattern matching rules. In addition to the automatic
# variables used here, the variable $* that matches whatever % stands for
# can be useful in special cases.
%.o: %.cpp
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

%: %.cpp
	$(CC) $(CFLAGS) -o -fPIC $@ $<

clean:
	-rm -f .depend *.o 

.PHONY: all build clean depend

