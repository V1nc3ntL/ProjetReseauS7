/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	businessLogicTypes.h : types représentant la    *
 *  logique métier                                  */
#ifndef __BUSINESS_LOGIC_TYPES__

typedef struct
{
    signed int amount;
    char* date;
} operations;

typedef struct
{
   char* accountId;
   signed int balance;
   operations* ops;
}  account;
// Un client à un Id, un password et des ids de compte
typedef struct
{
   char* id;
   char* pw;
   int nbAccount;
   account* accounts;
}  customer;


typedef struct{
    
    int nbCustomers;
    customer* c;
}
customerArray ;



#endif