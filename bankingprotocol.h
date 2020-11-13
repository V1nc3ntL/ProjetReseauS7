#ifndef __BANKING_PROTOCOL__

#define NB_CMD 5
#define ADD 0
#define WITHDRAWAL 1
#define BALANCE 2
#define OPERATIONS 3
#define CONNECTION 4

#define SEPARATORSTR ":"
#define SEPARATOR ':'
#define KO 1<<3
#define OK 2<<3
#define RES_SOLDE 3<<3
#define RES_OPERATION 4<<3
#define BUFFER_SIZE 512

#define AD_STR "Add"
#define CO_STR "Connect"
#define WI_STR "Withdraw"
#define BA_STR "Balance"
#define OP_STR "Operations"

const static char *cmds[NB_CMD] = { AD_STR, WI_STR, BA_STR, OP_STR, CO_STR };

#endif