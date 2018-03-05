OUT			= skip
CFLAGS 		= -Wall -g -O3
CPPFLAGS 	= $(CFLAGS) -std=c++11 -mrtm
LIBS		= -pthread -lbenchmark
CC			= gcc
CPP			= g++

CSRC		= $(wildcard *.c)
CPPSRC		= $(wildcard *.cpp)
OBJ			= $(CSRC:.c=.o) $(CPPSRC:.cpp=.o)

all: $(OUT)

$(OUT): $(OBJ)
		g++ -o $(OUT) $(OBJ) $(LIBS)

%.o: %.cpp
		$(CPP) $(CPPFLAGS) -c $^ -o $@

%.o: %.c
		$(CC) $(CFLAGS) -c $^ -o $@


