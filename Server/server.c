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
#include "../bankingprotocol.h"

#include "../businessLogic/customers.h"
#include "../businessLogic/accounts.h"
#define PORT 8080

//Mutex pour gestion des ressources
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Chaîne du client
static const char welcomeMsg[] =WELCOME_CLIENT_MESSAGE;

//Fichiers de clients et de comptes

static char* clientFileName = DEBUG_CLIENTF;
static char* accountFileName = DEBUG_ACCOUNTF;
static char* operationFileName = DEBUG_OPERATIONF;
// Données utilisateurs
static customerArray custs;


// Découpe la trame reçue
// tous les pointeurs doivent être NULL
void cutTrame(char* trame, char** id, char** accountId,char** pw,char**tmp,
int * amount){
  char mask ;
  char* empty;
  strtok_r(trame,SEPARATORSTR,id);
  strtok_r(*id,SEPARATORSTR,accountId);
  strtok_r(*accountId,SEPARATORSTR,pw);
  strtok_r(*pw,SEPARATORSTR,tmp);
  
  strtok_r(*tmp,SEPARATORSTR,&empty);

  *amount = 0 ;

  for(int i = 0 ; i < sizeof(int) ; i ++)
      *amount |= (((unsigned char) (tmp[0][i]) )<< i*8);

}

// Traitement des commandes
// Retourne la trame a envoyer
char* treatCommand(char* trame){

  unsigned int amount  = 0;
  int localId = 0 , lAccountId = -1,nbOp = 0;
  // Le code d'opération est sur les 4 premiers octets
  char* id = NULL,cmd = *trame, * pw=NULL,*tmp=NULL,*accountId=NULL,   
  // initialisation de la trame retour
  * toSend = NULL ,  
  opCode =  OP_CODE(trame); 
 
  toSend = malloc(2);
  
  *toSend=KO; 
  *(toSend+1)=0;
  

  //Récupération de la trame
  cutTrame(trame,&id,&accountId,&pw,&tmp,&amount);
  
  
  //Authentification du client
  localId = authenticate(&custs,id,pw);

  if( localId>=0){

    lAccountId = getAccountIndex(&custs,localId,accountId);
 
  if(lAccountId >= 0){

  fprintf (stdout,"\nCommande %s par %s sur %s" ,cmds[*trame],id,accountId); 

    *toSend = (char)OK;
     
    switch(opCode){
     
      case BALANCE:
        getBalance(custs,localId,lAccountId,&toSend);     

        break;
      break;

       case WITHDRAWAL:
        if(amount <= 0 ){
          fprintf(stderr,"Le chiffre doit être positif");
           *toSend |= (char)KO;
           break;
        }
        // Fermeture des ressources critiques
        pthread_mutex_lock(&mutex);
        custs.c[localId].accounts[lAccountId].balance -= amount;
        addOperation(&custs.c[localId].accounts[lAccountId],WITHDRAWAL,amount);
        pthread_mutex_unlock(&mutex);
 
        *toSend |= WITHDRAWAL;
        break;
    case OPERATIONS:
    case RES_OPERATION:
      getLastOperations(operationFileName,id,accountId,&toSend);     
      *toSend |= RES_OPERATION  ;
      break;
     case ADD:      

         if(amount<= 0 ){
          fprintf(stderr,"Le chiffre doit être positif");
          *toSend |= (char)KO;
          break;
        }
          pthread_mutex_lock(&mutex);
          custs.c[localId].accounts[lAccountId].balance += amount;
          addOperation(&(custs.c[localId].accounts[lAccountId]),ADD,amount);
          pthread_mutex_unlock(&mutex);
       * toSend |= ADD;
         
      break;
          default:
      break; 
  
  }
      pthread_mutex_lock(&mutex);
    fflush(stdout);
    updateOperationFile(&custs,operationFileName);
          pthread_mutex_unlock(&mutex);
  }  
  }
//    printf(cmds[*toSend]);
  return toSend;
}

void * 
treatTCPConnection (void *socket)
{
  char * tmp =NULL, opCode = KO,tmpChar = 0;
  char bufferTx[BUFFER_SIZE] = {0};
  char bufferRx[BUFFER_SIZE] = {0};

 int j = 0;
  pthread_mutex_lock (&mutex);
   //Envoie un message de bienvenue
  send (*(int *) socket, welcomeMsg, sizeof (welcomeMsg), MSG_NOSIGNAL);
  recv (*(int *) socket, bufferRx, BUFFER_SIZE, 0);
  
  pthread_mutex_unlock (&mutex);
  
  if(*bufferRx&KO){
    fprintf(stderr,"\nTrame non conforme reçue");
  }
  else{
    //On remet à 0
    bzero(bufferTx,  BUFFER_SIZE);

    tmp = treatCommand(bufferRx);

    if(*tmp == (RES_OPERATION |OK) ){
      memmove(bufferTx,tmp,NB_OPERATIONS*(RET_RES_OP_SZ) );
      bufferTx[0] = RES_OPERATION | OK;
    }
   
    else{
         if(*tmp & (RES_BAL )  ){
            memmove(bufferTx,tmp,BALANCE_SZ );
            bufferTx[0] |= RES_BAL | OK;
         }else
            strcpy(bufferTx,tmp);
    }

    if(*bufferTx & KO){
      fprintf(stderr,"\nErreur dans la création de la trame");
      send (*(int *) socket, bufferTx, BUFFER_SIZE, MSG_NOSIGNAL);
      bzero(bufferTx,BUFFER_SIZE);
      pthread_mutex_unlock (&mutex);  
    }
    else{
      pthread_mutex_lock (&mutex);
      updateAccountFile(&custs,accountFileName);
      if(*tmp & RES_OPERATION)
       send (*(int *) socket, bufferTx,NB_OPERATIONS*RET_RES_OP_SZ+NB_OPERATIONS, MSG_NOSIGNAL);
      else if(*tmp & RES_BAL)
       send (*(int *) socket, bufferTx, BALANCE_SZ, MSG_NOSIGNAL);
      else
        send (*(int *) socket, bufferTx, BUFFER_SIZE, MSG_NOSIGNAL);
      
      pthread_mutex_unlock (&mutex);  
      bzero(bufferTx,BUFFER_SIZE);
    }

  free(tmp);
  }
  printf("\n");
}

// Permet d'intercepter l'arrêt
void
SIGINTHandler (int dummy)
{
	printf("\nArrêt du serveur TCP\n");
	fflush(stdout);
  liberateCustomerArray(custs.c,custs.nbCustomers);
  exit(EXIT_FAILURE);
}

//Fonction pour thread TCP
void *
launchUDPServer (void *server)
{
  pthread_t thread_id[BUFFER_SIZE];
  struct sockaddr_in servaddr, cliaddr;
  printf ("Launching UDP banking server on port %d\n", *(int*)server);

  // Creation du socket UDP IPv4
   if ( ( *(int*)server = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        fprintf(stderr,"socket non créé"); 
        exit(EXIT_FAILURE); 
    } 
    // Mise à zéro des adresses
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 

    // Remplissage des informations pour le serveur
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = *(int*)server; 
    
    // Liaison entre le socket et le port
    if (bind(*(int*)server , (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        fprintf(stderr,"socket non lié"); 
        exit(EXIT_FAILURE); 
    } 

  while(1){
    
  }

}
//Fonction pour thread TCP
void *
launchTCPServer (void *server)
{
  pthread_t thread_id[BUFFER_SIZE];
  struct sockaddr_in address;

  int i = 0, j = 0, opt = 1, addrlen = sizeof (address), clientSock = 0;

  printf ("Launching TCP banking server on port %d\n", *(int*)server);
  

  // Creation du socket TCP IPv4
  if ((*(int *) server = socket (AF_INET, SOCK_STREAM, 0)) == 0)
    {
      fprintf (stderr,"Probleme dans la creation du socket");
      exit (EXIT_FAILURE);
    }
// Indique que le socket peut être connecté plusieurs fois
  if (setsockopt (*(int *) server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		  &opt, sizeof (opt)))
    {
      fprintf (stderr,"setsockopt");
      exit (EXIT_FAILURE);
    }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port =  htons(PORT);

  // Lie le serveur à l'adresse PORT
  if (bind (*(int *) server, (struct sockaddr *) &address, sizeof (address)) <
      0)
    {
      fprintf (stderr,"bind failed");
      exit (EXIT_FAILURE);
    }

  // On fixe le nombre maximum de connexion 
  if (listen (*(int *) server, NB_MAX_CO) < 0)
    {
      fprintf (stderr,"listen");
      exit (EXIT_FAILURE);
    }
  
  while (1)
  {
  
    if(clientSock = accept (*(int *) server, (struct sockaddr *) &address,
			      (socklen_t *) & addrlen))
    {
      if (pthread_create
	  (&thread_id[i++], NULL, treatTCPConnection,
	   (void *) &clientSock) < 0){
      exit (EXIT_FAILURE);
     }	   
    }
    for (j = 0; j < i; j++)
    {
      pthread_join (thread_id[j], NULL);
    }
  }
}

int
main (int argc, char const *argv[])
{
  int *server, port = PORT;

  pthread_t udpThread,tcpThread;

  //association du ctrl-c à la libération de mémoire
  signal (SIGINT, SIGINTHandler);

  getClientsAndAccountFrom (clientFileName, accountFileName, &custs);

	display(&custs);
  fflush (stdout);

  if (!pthread_create (&tcpThread, NULL, launchTCPServer, (void *) &port),
      (void *) &custs)
    pthread_join (tcpThread, NULL);

  if (!pthread_create (&udpThread, NULL, launchUDPServer, (void *) &port),
      (void *) &custs)
    pthread_join (udpThread, NULL);
   

  liberateCustomerArray (custs.c, custs.nbCustomers);
  return 0;
}
