ARMGCC=aarch64-linux-gnu-g++
X64GCC=gcc

all: server client

server:
	$(ARMGCC) -o server server.c -Wall 

client:
	$(X64GCC) -o client client.c -Wall

clean:
	rm -f server client
