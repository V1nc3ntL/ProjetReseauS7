/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	businessLogicTypes.h : types représentant la    *
 *  logique métier                                  */
#ifndef __BUSINESS_LOGIC_TYPES__

typedef struct
{
  // On peut déterminer le - selon le type
  int amount;
  char type;
  char *date;
} operation;

typedef struct
{
  char *accountId;
  int balance;
  int nbOps;
  //Permet de garder une mémoire contigüe
  operation *ops;
} account;
// Un client à un Id, un password et des ids de compte
typedef struct
{
  char *id;
  char *pw;
  int nbAccount;
  account *accounts;
} customer;


typedef struct
{

  int nbCustomers;
  customer *c;
}
customerArray;



#endif
