/*	EL-KHARROUBI 	LEFEBVRE						*
 *	EISE4											*
 *	Reseaux											*
 *	TP4												*
 *	client.c : Application client distant			*/
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "bankingprotocol.h"
#include "client.h"
#define PORT 8080


int
getOperationCode (char *argCmd)
{
  int toReturn = 0;


  for (toReturn = 0; toReturn < NB_CMD; toReturn++)
    {
      if (!strcmp (cmds[toReturn], argCmd))
	break;
    }

  if (toReturn == NB_CMD)
    toReturn = KO;

  return toReturn;
}
char * allocateTrame(int nbArg,char*args[],char trameHeader){
  int i = 0;
  char* toSend = NULL;

  ssize_t szToAlloc = 1;
  
  for(i=1; i < nbArg ; i++){
    szToAlloc += strlen(args[i]);
  } 

  toSend = malloc(szToAlloc+=4);
  toSend[0] = trameHeader ;
  toSend[1] = SEPARATOR;
  
  szToAlloc= 2;
  
  for(i = 1; i < nbArg+1 ;i++){

    memmove(toSend+szToAlloc,args[i],strlen(args[i]));
    szToAlloc += strlen(args[i])+1;
    toSend[szToAlloc-1] = SEPARATOR;
    
  }

  
  return toSend;
}



int
main (int argc, char const *argv[])
{
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char *hello = "Hello from client";
  char rxBuf[BUFFER_SIZE], buffer[BUFFER_SIZE] = { 0 };
  char *cmd = NULL;
  //Les commandes sont créées avec 4 arguments max
  char arg1[50], arg2[50], arg3[50], arg4[50];
  char* args[4] = {arg1,arg2,arg3,arg4};
  char trameHeader = 0, nbArg = 0;
  char *toSend = NULL;


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
      nbArg = sscanf (buffer, "%s %s %s %s", arg1, arg2, arg3, arg4);
      trameHeader = getOperationCode (arg1);
      printf ("Trame header : %d\n", trameHeader);
      if (trameHeader & KO)
	{
	  printf ("\nCommande non reconnue\n");
	  fflush (stdout);
	}
      else
	{
    trameHeader |= OK;
    printf ("Trame header : %d\n",trameHeader);

	  fflush (stdout);	
	
      toSend = allocateTrame(nbArg,args,trameHeader);

      printf ("\nCommande %s envoyée\n", cmds[toSend[0] & 0x7]);
      
      send (sock, toSend, strlen (toSend) , 0);
      printf("\nRentrez une nouvelle commande :\n");
      fflush (stdout);
      bzero(buffer,BUFFER_SIZE);
      free (toSend);
	    
	}
    }


  return 0;
}
