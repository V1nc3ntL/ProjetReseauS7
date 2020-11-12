#include "customers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bankingprotocol.h"

int liberateCustomerArray(customer * custs,int nbCustomers ){
	int i = 0,j=0;
	for(i = 0 ; i< nbCustomers;i++){
		   	free(custs[i].id);
    		free(custs[i].pw);
			for(j = 0 ; j <custs[i].nbAccount ; j ++ ){
				free(custs[i].accounts[j].accountId);
			}
			free(custs[i].accounts);
	}
	free(custs);
}

// Récupère les comptes d'un FILE * ouvert en mode lecture
void getAccountsFrom(FILE * fa,customerArray * custs){
	int nChar,i = 0;
	ssize_t bufS = BUFFER_SIZE ;
	ssize_t szToAlloc =0;
	char * accountId = NULL, *tmp = NULL,*bal = NULL;
	int solde = 0;
	account * copy = NULL;
	nChar = getline(&accountId,&bufS,fa);

		while(nChar>=0){
		
		strtok_r(accountId, SEPARATOR,&tmp);	
		for(i =0 ; i < custs->nbCustomers;i++){

			// On a trouvé le proprietaire du compte
			if(! strcmp(custs->c[i].id,accountId)){
					
					custs->c[i].nbAccount++;
					strtok_r(tmp, SEPARATOR,&bal);

					//	printf(tmp);
					szToAlloc = sizeof(custs->c[i].accounts)+sizeof(account)+sizeof(int);
					copy = NULL;
					while(!copy)
						copy = realloc(custs->c[i].accounts, szToAlloc);
					
					custs->c[i].accounts = copy;
					custs->c[i].accounts[custs->c[i].nbAccount-1].accountId = malloc(strlen(tmp)+1);
					memmove(custs->c[i].accounts[custs->c[i].nbAccount-1].accountId,tmp,strlen(tmp)+1);
					custs->c[i].accounts[custs->c[i].nbAccount-1].balance = atoi(bal) ;
			}
		}
			nChar = getline(&accountId,&bufS,fa);
		}

	free(accountId);
}



// Crée le tableau de clients à partir d'un FILE * ouvert en lecture
size_t createCustDatabase(customerArray * custs,FILE* fc,char* id, char *pw){
	ssize_t szToAlloc;
	ssize_t bufS = BUFFER_SIZE ;
	int nCharC,i = 0;
	customer * copy = NULL;

	custs->c = NULL;
	custs->nbCustomers = 0;
	
	nCharC = getline(&id,&bufS,fc);
    szToAlloc = (size_t)sizeof(custs->c);
		while(nCharC>=0){
		
		strtok_r(id, SEPARATOR,&pw);	
		szToAlloc += sizeof(customer)+strlen(id)+2+strlen(pw)+sizeof(int);
        copy = NULL;
        while(!copy){
            copy = realloc(custs->c,szToAlloc);
        }
        custs->c= copy;
        custs->c[i].id = malloc(strlen(id)+1);
        custs->c[i].pw = malloc(strlen(pw)+1);
        memmove(custs->c[i].id,id,(size_t)strlen(id)+1);
		memmove(custs->c[i].pw,pw,(size_t)strlen(pw)+1);  
		custs->c[i].nbAccount = 0;
		custs->c[i].accounts = NULL;
		nCharC = getline(&id,&bufS,fc);
		i++;
	}
	custs->nbCustomers = i ;

	free(id);
	return szToAlloc;
}

int getClientsAndAccountFrom(const char* clientFilename,const char* accountFileName,
						 customerArray * custs ){

	char* id = NULL,*pw =NULL;
	ssize_t szToAlloc;
	FILE* fc = fopen(clientFilename,"r"),*fa = fopen(accountFileName,"r");
	
	if(!fc){
		fprintf(stderr,"Fichier de clients manquants");
		return EXIT_FAILURE;
	}
	szToAlloc = createCustDatabase(custs,fc,id, pw);

	getAccountsFrom(fa,custs);

	free(id);
	free(pw);
	fclose(fc);
	fclose(fa);
}


