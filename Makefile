# Use g++ for compilation
CC=g++

# Compile with c++11
CFLAGS= -c -Wall --std=c++11

all: file_handling

file_handling: file_handling.o
	$(CC) file_handling.o -o file_handling

file_handling.o: file_handling.cpp
	$(CC) $(CFLAGS) file_handling.cpp

clean:
	rm -rf *.o
