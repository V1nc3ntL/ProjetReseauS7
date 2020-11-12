#ifndef __BANKING_PROTOCOL__

#define NB_CMD 5
#define ADD 0
#define WITHDRAWAL 1
#define BALANCE 2
#define OPERATIONS 3
#define CONNECTION 4

#define SEPARATOR ":"
#define KO 1<<3
#define OK 2<<3
#define RES_SOLDE 3<<3
#define RES_OPERATION 4<<3
#define BUFFER_SIZE 512

#endif