
/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	accounts.h : Déclaration des fonctions sur      *
 *les comptes			                            */
#ifndef __ACCOUNTS__

#define DATE_FORMAT_SZ 28
#define BALANCE_SZ 6
int getLastOperations (char *operationFile, char *id, char *accountId,
		       char **headerOnly);
int updateOperationFile (customerArray * custs, char *opF);
int updateAccountFile (customerArray * custs, char *aF);
void updateOperations (account * account);
void displayOperations (account * account);
void addOperation (account * act, char opCode, int sum);
int removeFromAccount (customerArray * custs, int customerIndex,
		       int accountIndex, signed int amount);
int addToAccount (customerArray * custs, int customerIndex, int accountIndex,
		  signed int amount);
void getBalance(customerArray custs,int customerIndex, int accountIndex,char ** toSend); 
#endif
