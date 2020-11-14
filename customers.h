/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	customers.h : définition de fonction de         *
 *  manipulation des clients                        */
#ifndef __CUSTOMERS__
#include "businesslogictypes.h"
#define DEBUG_ACCOUNTF "accountFile"
#define DEBUG_CLIENTF "clientFile"



int authenticate(customerArray * custs,char** trame);
int liberateCustomer(customer cus);
int liberateCustomerArray(customer * custs,int nbCustomers );
int getClientsAndAccountFrom(const char* clientFilename,const char* accountFileName,
						 customerArray * custs );
void display(customerArray * custs);
#endif