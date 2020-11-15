
/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	accounts.h : Déclaration des fonctions sur      *
 *les comptes			                            */
#ifndef __ACCOUNTS__

#define DATE_FORMAT_SZ 29

void addOperation(account * act,char opCode,int sum);
int removeFromAccount(customerArray * custs,int customerIndex,int accountIndex,signed int amount );
int addToAccount(customerArray * custs,int customerIndex,int accountIndex,signed int amount );
#endif