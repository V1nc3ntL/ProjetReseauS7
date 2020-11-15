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
#include "accounts.h"
#define PORT 8080
//Mutex pour gestion des ressources
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Chaîne du client
static const char welcomeMsg[] = WELCOME_CLIENT_MESSAGE;

//Fichiers de clients et de comptes

static char* clientFileName = DEBUG_CLIENTF;
static char* accountFileName = DEBUG_ACCOUNTF;

static customerArray custs;
/*
int updateClientFile(customerArray* custs,char* clientF){
  FILE * cF = fopen(clientF,"w");
  int ret;
  char* tmp; 
  if(!cF){
    fprintf(stderr,"Impossible d'ouvrir le fichier client %s",clientF);
    ret = EXIT_FAILURE;
    
  }
  else{
    for(i=)
    getline()
    fclose(cF);
  }


  return ret;
}*/
int updateAccountFile(customerArray* custs,char* accountF){
  FILE * aF = fopen(accountF,"w");
  int ret , i , j;
  char tmp[BUFFER_SIZE]; 
  
  if(!aF){
    fprintf(stderr,"Impossible d'ouvrir le fichier compte %s",accountF);
    ret = EXIT_FAILURE;
    
  }
  else{
    for(i = 0; i < custs->nbCustomers ; i++){
      for(j = 0 ; j < custs->c[i].nbAccount;j++){
        fputs( custs->c[i].id,aF);
        fputc(SEPARATOR,aF);
        fputs( custs->c[i].accounts[j].accountId ,aF);
        fputc(SEPARATOR,aF);
        sprintf(tmp,"%d",custs->c[i].accounts[j].balance);
        fputs( tmp,aF);
        bzero(tmp,BUFFER_SIZE);
        fputc('\n',aF);
      }
        

    }
    fclose(aF);
  }


  return ret;
}
// Découpe la trame reçue
// tous les pointeurs doivent être NULL
void cutTrame(char** trame, char** id, char** accountId,char** pw,char**tmp){
  strtok_r(*trame,SEPARATORSTR,id);
  strtok_r(*id,SEPARATORSTR,accountId);
  strtok_r(*accountId,SEPARATORSTR,pw);
  strtok_r(*pw,SEPARATORSTR,tmp);
  //strtok_r()
}

// Traitement des commandes
// Retourne la trame a envoyer
char* treatCommand(char* trame){
  int localId = 0 ,amount=0, lAccountId = 0;
  char cmd = *trame, 
  // Le code d'opération est sur les 4 premiers octets
  opCode =  *trame & 0x07;

  char* id, * pw,*tmp,*accountId;

  char * ok = malloc(OK);
  /*
  printf ("Commande %s reçue\n",cmds[opCode]); 
 */
  //Récupération de la trame
  cutTrame(&trame,&id,&accountId,&pw,&tmp);
 /*
    printf("\n Id = %s",id);
   printf("\n pw = %s",pw);

   printf("\n pw = %s",tmp);*/
  localId = authenticate(&custs,id,pw);

  if( localId<0)
    exit(EXIT_FAILURE);

  printf("\n\t\tLOCAL ID : %d\n\t\t%s",localId,custs.c[localId].id);
  
  lAccountId = getAccountIndex(&custs,localId,accountId);
  printf("\n\t\t%s",custs.c[localId].accounts[lAccountId].accountId);
  printf("\nlAccountId =  %d",lAccountId);
    
  switch((char)opCode){
    case ADD:
         if(atoi(tmp) <= 0 ){
          fprintf(stderr,"Le chiffre doit être positif");

        }
        else{
       
          pthread_mutex_lock (&mutex);
          addToAccount(&custs,localId,lAccountId, atoi(tmp));
          pthread_mutex_unlock (&mutex);
          display(&custs);
        }
      break;
    case WITHDRAWAL:
        if(atoi(tmp) <= 0 ){
          fprintf(stderr,"Le chiffre doit être positif");
        }
        pthread_mutex_lock (&mutex);
        removeFromAccount(&custs,localId,lAccountId, abs(atoi(tmp)));
        pthread_mutex_unlock (&mutex);
        display(&custs);

      //withdrawFromAccount(custs);
      break;
    case BALANCE:
      //getBalance();
      break;
    case OPERATIONS:
      //getOperations();
      break;
    case CONNECTION:
    
      break;   

  }
  return ok;

}

void * 
treatTCPConnexion (void *socket)
{
  char * tmp , *bufferTx,opCode = KO;
  char bufferRx[BUFFER_SIZE] = { 0 };

  //Envoie un message de bienvenue
  pthread_mutex_lock (&mutex);
  send (*(int *) socket, welcomeMsg, sizeof (welcomeMsg), 0);
  pthread_mutex_unlock (&mutex);

  
  //Protection des ressources critiques
  pthread_mutex_lock (&mutex);
  recv (*(int *) socket, bufferRx, BUFFER_SIZE, 0);
  pthread_mutex_unlock (&mutex);

  if(*bufferRx&KO){
    fprintf(stderr,"\nTrame non conforme reçue");
  }
  else{
    printf ("%s reçu\n",bufferRx);
    //On traite sans le Header
    
    bufferTx = treatCommand(bufferRx);
    pthread_mutex_lock (&mutex);
    updateAccountFile(&custs,accountFileName);
    send (*(int *) socket, bufferTx, strlen (bufferTx), 0);
    pthread_mutex_unlock (&mutex);  
  }
    


}

// Permet d'effectuer des actions de libération
// mémoire lors de ctrl c
void
TCPIntHandler (int dummy)
{
	printf("\nArrêt du serveur TCP\n");
	fflush(stdout);
	liberateCustomerArray (custs.c, custs.nbCustomers);
  exit(EXIT_FAILURE);

}
//Fonction pour thread TCP
void *
launchUDPServer (void *server)
{}
//Fonction pour thread TCP
void *
launchTCPServer (void *server)
{
  pthread_t thread_id[BUFFER_SIZE];
  struct sockaddr_in address;

  int i = 0, j = 0, opt = 1, addrlen = sizeof (address), clientSock;

  //association du ctrl-c à la libération de mémoire
  signal (SIGINT, TCPIntHandler);

  // Creation du socket TCP IPv4
  if ((*(int *) server = socket (AF_INET, SOCK_STREAM, 0)) == 0)
    {
      perror ("Probleme dans la creation du socket");
      exit (EXIT_FAILURE);
    }
// Indique que le socket peut être connecté plusieurs fois
  if (setsockopt (*(int *) server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		  &opt, sizeof (opt)))
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (PORT);

  // Lie le serveur à l'adresse PORT
  if (bind (*(int *) server, (struct sockaddr *) &address, sizeof (address)) <
      0)
    {
      perror ("bind failed");
      exit (EXIT_FAILURE);
    }

  // On fixe le nombre maximum de connexion 
  if (listen (*(int *) server, NB_MAX_CO) < 0)
    {
      perror ("listen");
      exit (EXIT_FAILURE);
    }
  while (clientSock = accept (*(int *) server, (struct sockaddr *) &address,
			      (socklen_t *) & addrlen))
    {
      if (pthread_create
	  (&thread_id[i++], NULL, treatTCPConnexion,
	   (void *) &clientSock) < 0)
	exit (EXIT_FAILURE);

    }
  for (j = 0; j < i; j++)
    {
      pthread_join (thread_id[j], NULL);
    }

}

int
main (int argc, char const *argv[])
{
  int *server, port = PORT;

  pthread_t udpThread,tcpThread;


  getClientsAndAccountFrom (clientFileName, accountFileName, &custs);

	display(&custs);

  printf ("Launching banking server on port %d\n", port);
  
  fflush (stdout);

  if (!pthread_create (&tcpThread, NULL, launchTCPServer, (void *) &port),
      (void *) &custs)
    pthread_join (tcpThread, NULL);

  if (!pthread_create (&udpThread, NULL, launchUDPServer, (void *) &port),
      (void *) &custs)
    pthread_join (udpThread, NULL);


    display(&custs);


  liberateCustomerArray (custs.c, custs.nbCustomers);
  return 0;
}
