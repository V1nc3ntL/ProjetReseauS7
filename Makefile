CC=gcc
CCFLAGS= -Wall -Werror 
LIBFLAGS=-lpthread
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)
TST_DIR=tests/
TST= $(wildcard $(TST_DIR)/*.cc)
OBJ_TEST = $(filter-out main.o, $(OBJ)) $(TST:.c=.o)
EXECS="server client" 



all: server client 

clean: 
	rm -rf *.o vgcore*

client: client.c
	$(CC) $(LIBFLAGS) $^ -o $@  

server: server.c customers.c accounts.c
	$(CC) $(LIBFLAGS) $^ -o $@  
	
