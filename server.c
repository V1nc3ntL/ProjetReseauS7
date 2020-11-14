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


static customerArray custs;


void treatCommand(char* trame){
  int localId = 0 ,amount=0;
  char cmd = *trame, opCode =  *trame & 0x07;
  char* id, * pw,*tmp,*accountId;

  printf ("Commande %s reçue\n",cmds[opCode]); 
  strtok_r(trame,SEPARATORSTR,&id);
  localId = authenticate(&custs,&id);

   if(localId<0)
    exit(EXIT_FAILURE);
    
  switch((char)opCode){
    case ADD:
      
        strtok_r(id,SEPARATORSTR,&pw);
        pthread_mutex_lock (&mutex);
        addToAccount(&custs,localId,id, atoi(pw));
        pthread_mutex_unlock (&mutex);
        display(&custs);
      
      break;
    case WITHDRAWAL:

        strtok_r(id,SEPARATORSTR,&pw);
        pthread_mutex_lock (&mutex);
        removeFromAccount(&custs,localId,id, atoi(pw));
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
}

void *
treatTCPConnexion (void *socket)
{
  char buffer[BUFFER_SIZE] = { 0 };
  char * tmp;
  char opCode = KO;
  static const char welcomeMsg[] = WELCOME_CLIENT_MESSAGE;
  send (*(int *) socket, welcomeMsg, sizeof (welcomeMsg), 0);


  while (1)
    {
		//Protection des ressources critiques
      pthread_mutex_lock (&mutex);
      recv (*(int *) socket, buffer, BUFFER_SIZE, 0);
      pthread_mutex_unlock (&mutex);
	

      if(*buffer&KO){
        fprintf(stderr,"\nTrame non conforme reçue");
      }
      else{

      
	    fflush (stdout);
      printf ("%s reçu\n",buffer);
      //On traite sans le Header
      treatCommand(buffer);

      pthread_mutex_lock (&mutex);
      send (*(int *) socket, buffer, strlen (buffer), 0);
	    pthread_mutex_unlock (&mutex);  
    
      }
    }

}

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

  pthread_t tcpThread;

  if (argc == 4)
    {
      getClientsAndAccountFrom (argv[2], argv[3], &custs);
    }
  else
    getClientsAndAccountFrom (DEBUG_CLIENTF, DEBUG_ACCOUNTF, &custs);

	display(&custs);
  printf ("Launching banking server on port %d\n", port);
  
  fflush (stdout);

  if (!pthread_create (&tcpThread, NULL, launchTCPServer, (void *) &port),
      (void *) &custs)
    pthread_join (tcpThread, NULL);

  liberateCustomerArray (custs.c, custs.nbCustomers);
  return 0;
}
