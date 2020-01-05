CC = mpicc
VPATH = src INCLUDE

all: program

program: main.o
	mkdir -p build
	mpicc -o build/main.o src/main.c -pthread -lcrypt -lcrypto
	rm -f *.o

clean:
	rm -rf build
