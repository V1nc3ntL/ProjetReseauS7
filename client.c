// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "bankingprotocol.h"
#include "client.h"
#define PORT 8080


int
getOperationCode (char *argCmd)
{
  int toReturn = 0;
  char *cmds[NB_CMD] = { AD_STR, WI_STR, BA_STR, OP_STR, CO_STR };

  for (toReturn = 0; toReturn < NB_CMD; toReturn++)
    {
      if (!strcmp (cmds[toReturn], argCmd))
	break;
    }
  if (toReturn == NB_CMD)
    toReturn = KO;

  return toReturn;
}

int
main (int argc, char const *argv[])
{
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char *hello = "Hello from client";
  char rxBuf[BUFFER_SIZE], buffer[BUFFER_SIZE] = { 0 };
  char *cmd = NULL, *args = NULL;
  //Les commandes sont créées avec 4 arguments max
  char arg1[50], arg2[50], arg3[50], arg4[50];
  int trameHeader = 0;
  char *localArgv[4] = {
    arg1, arg2, arg3, arg4
  };
  if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf ("\nProbl\203me \n");
      return -1;
    }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons (PORT);
  //Connecte à local host, on doit récupérer
  // L'IP dans une chaîne 
  // Convert IPv4 and IPv6 addresses from text to binary form 
  if (inet_pton (AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
      printf ("\nInvalid address/ Address not supported \n");
      return -1;
    }

  // Mode connecté
  if (connect (sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
    {
      printf ("\nConnection Failed \n");
      return -1;
    }

  while (1)
    {
      read (sock, buffer, BUFFER_SIZE);
      printf ("%s\n", buffer);
      bzero (buffer, BUFFER_SIZE);
      fgets (buffer, BUFFER_SIZE, stdin);
      sscanf (buffer, "%s %s %s %s", arg1, arg2, arg3, arg4);
      trameHeader = getOperationCode (arg1);

      printf ("Trame header : %d\n", trameHeader);
      printf ("Trame header : %d", KO);
      fflush (stdout);
      //Vérification de la commande
      if (trameHeader & KO)
	{
	  printf ("\nCommande non reconnue\n");
	  fflush (stdout);
	}
      else
	{

	  send (sock, arg1, strlen (rxBuf), 0);
	}
    }
  while (1)
    {
      recv (sock, rxBuf, BUFFER_SIZE, 0);

      send (sock, rxBuf, strlen (rxBuf), 0);
    }


return 0;
}

