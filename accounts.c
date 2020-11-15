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
#include <time.h>

#include "server.h"
#include "bankingprotocol.h"
#include "customers.h"
#include "accounts.h"

#define PORT 8080
//Mutex pour gestion des ressources
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//char* opToStr()

void displayOperations(account * account){
    printf("%d",account->nbOps);
    for(int i = 0 ; i < account->nbOps;i++){
        printf("\n%s",account->ops[i].date);
        printf(cmds[account->ops[i].type]);
        printf("%d",account->ops[i].amount);
    }      
}
void addOperation(account * act,char opCode,int amount){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char dateStr[DATE_FORMAT_SZ];
    operation* copy;
    //operation * localOperation = realloc(account->ops,sizeof(account->ops)+32);

    sprintf(dateStr,"%04d/%02d/%02d:%02d-%02d-%02d", 
        tm.tm_year + 1900,
        tm.tm_mon + 1, 
        tm.tm_mday, 
        tm.tm_hour, 
        tm.tm_min, 
        tm.tm_sec);
    printf(dateStr);

   /*

    if(!act->nbOps){
        act->ops = malloc(sizeof(operation)+DATE_FORMAT_SZ);
        
        printf("PAS D'OP DE BASE");
        act->ops->amount = amount;
      act->ops->type = opCode;
      
      memmove(act->ops->date , dateStr,DATE_FORMAT_SZ);
        act->nbOps++;
        displayOperations(act);
    
    }/*else{
        act->ops[act->nbOps-1].date = malloc(DATE_FORMAT_SZ);
        //local = malloc(sizeof(operation));
         printf("OP DEJA");
        displayOperations(act);
        printf("PAS D'OP DE BASE");
        copy = NULL;

        while(!copy)
            copy = realloc(act->ops,sizeof(act->ops)+sizeof(operation)+DATE_FORMAT_SZ);
        
        act->ops = copy;
        act->ops[act->nbOps-1].amount = amount;
        act->ops[act->nbOps-1].type = opCode;
        memmove(act->ops[act->nbOps-1].date ,dateStr,DATE_FORMAT_SZ);

        act->nbOps++;   
    }
    //printf("NB OPS = %d",account->nbOps);
    /*account->ops[account->nbOps].amount = amount;
    account->ops[account->nbOps].type = opCode;
    memmove(account->ops[account->nbOps].date , dateStr,DATE_FORMAT_SZ);
 
    */
  

}

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
int addToAccount(customerArray * custs,int customerIndex,int accountIndex,signed int amount ){
        custs->c[customerIndex].accounts[accountIndex].balance += amount;


    return EXIT_FAILURE;
}



//Ajoute une somme sur le compte id
int removeFromAccount(customerArray * custs,int customerIndex,int accountIndex,signed int amount ){
         custs->c[customerIndex].accounts[accountIndex].balance -= amount;
            //custs->c[customerIndex].accounts[accountIndex].accounts.

    return EXIT_SUCCESS;
}