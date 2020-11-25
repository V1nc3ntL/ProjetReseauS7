CC=gcc
CCFLAGS= -Wall -Werror 
LIBFLAGS=-lpthread -lm
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)
TST_DIR=tests/
TST= $(wildcard $(TST_DIR)/*.cc)
OBJ_TEST = $(filter-out main.o, $(OBJ)) $(TST:.c=.o)
EXECS=server client



all: server client 

clean: 
	rm -rf *.o vgcore* $(EXECS)

client: client.c
	$(CC) $(LIBFLAGS) $^ -o $@  

server: server.c businessLogic/customers.c businessLogic/accounts.c
	$(CC) -g $(LIBFLAGS) $^ -o $@  
	
