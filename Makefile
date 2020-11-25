CC=gcc
CCFLAGS= -Wall -Werror 
LIBFLAGS=-lpthread -lm
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)
TST_DIR=tests/
TST= $(wildcard $(TST_DIR)/*.cc)
OBJ_TEST = $(filter-out main.o, $(OBJ)) $(TST:.c=.o)
EXECS=server client
BSL=./businessLogic/
all: $(EXECS)

clean: 
	rm -rf *.o vgcore* $(EXECS)

client: Client/client.c
	$(CC) $(LIBFLAGS) $^ -o $@  

server: Server/server.c ${BSL}customers.c ${BSL}accounts.c
	$(CC) -g $(LIBFLAGS) $^ -o $@  
	
