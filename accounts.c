/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	server.c : définitions des fonctions du serveur */
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "server.h"
#include "bankingprotocol.h"
#include "customers.h"

#define PORT 8080
//Mutex pour gestion des ressources
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#include "accounts.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int findAccountIndex(customerArray * custs,int customerIndex,char* accountId){
  int i ;

    for(i=0;i<custs->c[customerIndex].nbAccount;i++)
        if(!strncmp(accountId,custs->c[customerIndex].accounts[i].accountId,strlen(accountId)))
            break;

    if(i == custs->c[customerIndex].nbAccount){
        fprintf(stderr,"\nAucun compte ne correspond\n");
        i=-1;
    }
    return i;
}

//Ajoute une somme sur le compte id
int addToAccount(customerArray * custs,int customerIndex,char* accountId,signed int amount ){
    int i = findAccountIndex(custs,customerIndex,accountId);
    if(i >= 0)
        custs->c[customerIndex].accounts[i].balance += amount;
    
    return i;
}



//Ajoute une somme sur le compte id
int removeFromAccount(customerArray * custs,int customerIndex,char* accountId,signed int amount ){
    int i = findAccountIndex(custs,customerIndex,accountId);
    if(i >= 0)
         custs->c[customerIndex].accounts[i].balance -= amount;

    return i;
}