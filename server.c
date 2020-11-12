// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include <pthread.h>
#include "server.h"

#include"bankingprotocol.h"
#include"customers.h"
#define PORT 8080
//Mutex pour gestion des ressources
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
treatConnexion (void *socket)
{
  char buffer[BUFFER_SIZE] = { 0 };
  static const char welcomeMsg[] = WELCOME_CLIENT_MESSAGE;
  send (*(int *) socket, welcomeMsg, sizeof (welcomeMsg), 0);


  while (1)
    {
      pthread_mutex_lock (&mutex);
      recv (*(int *) socket, buffer, BUFFER_SIZE, 0);
      send (*(int *) socket, buffer, strlen (buffer), 0);
      printf (buffer);
      fflush (stdout);
      recv (*(int *) socket, buffer, BUFFER_SIZE, 0);
      printf (buffer);
      fflush (stdout);
      pthread_mutex_unlock (&mutex);

      break;
    }

}


int
main (int argc, char const *argv[])
{
  int server, clientSock, valread, opt = 1, i = 0, j = 0;
  struct sockaddr_in address;
  int addrlen = sizeof (address);
  customerArray custs;

  // Le nombre max de thread UNIX est 1024
  pthread_t thread_id[BUFFER_SIZE * 4];

  if (argc == 4)
    {
      getClientsAndAccountFrom (argv[2], argv[3], &custs);
    }
  else
    getClientsAndAccountFrom (DEBUG_CLIENTF, DEBUG_ACCOUNTF, &custs);

  printf ("Launching banking server on port %d\n", PORT);
  fflush (stdout);

  // Creation du socket TCP IPv4
  if ((server = socket (AF_INET, SOCK_STREAM, 0)) == 0)
    {
      perror ("Probleme dans la creation du socket");
      exit (EXIT_FAILURE);
    }


  // Indique que le socket peut être connecté plusieurs fois
  if (setsockopt (server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		  &opt, sizeof (opt)))
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (PORT);

  // Lie le serveur à l'adresse PORT
  if (bind (server, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
      perror ("bind failed");
      exit (EXIT_FAILURE);
    }

  // On fixe le nombre maximum de connexion 
  if (listen (server, NB_MAX_CO) < 0)
    {
      perror ("listen");
      exit (EXIT_FAILURE);
    }

  while (clientSock = accept (server, (struct sockaddr *) &address,
			      (socklen_t *) & addrlen))
    {
      if (pthread_create
	  (&thread_id[i++], NULL, treatConnexion, (void *) &clientSock) < 0)
	return 1;
    }

  for (j = 0; j < i; j++)
    {
      pthread_join (thread_id[j], NULL);
    }

  liberateCustomerArray (custs.c, custs.nbCustomers);
  return 0;
}
