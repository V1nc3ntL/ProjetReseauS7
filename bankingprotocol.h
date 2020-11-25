/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	businessLogicTypes.h : définitions et macros    *
 *  pour le protocole                               */

#ifndef __BANKING_PROTOCOL__

#define HEADER_SIZE 1


#define NB_CMD 5
// Port pour le protocole
#define PORT 8080
//Premier octet de trame

//Les 3 1ers bits donnent l'opération
#define OP_CODE(a) ((*a) & 0x07)
#define ADD 0
#define WITHDRAWAL 1
#define BALANCE 2
#define RES_OPERATION 3
#define OPERATIONS 4

// 4 bits pour le code de retour de trame

#define KO 1<<3
#define OK 1<<4
#define RES_BAL 1<<5
#define NEG 1<<6

// Second octet de statut de la trame
// Identifie le n° d'opération en cas
// de demandes d'opérations
#define STATUS(a) (*(a+1) &  0xf0);


#define TIME 6
#define RET_RES_OP_SZ 18

#define END_OPERATION


#define BUFFER_SIZE 512

#define AD_STR "AJOUT"
#define CO_STR "CONNECT"
#define WI_STR "RETRAIT"
#define BA_STR "SOLDE"
#define OP_STR "OPERATIONS"
#define RES_STR "RESULTATOPERATION"
#define SEPARATORSTR ":"
#define SEPARATOR ':'


#define NB_OPERATIONS 10
const static char *cmds[NB_CMD] = { AD_STR, WI_STR, BA_STR, OP_STR, CO_STR };

#endif
