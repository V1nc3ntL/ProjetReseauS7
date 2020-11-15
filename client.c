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

#define CMD_BUFFER_SIZE 25
// Retourne le code opération du header
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
//Retourne un char* avec la chaîne à envoyer
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
  
  for(i = 1; i < nbArg ;i++){

    memmove(toSend+szToAlloc,args[i],strlen(args[i]));
    szToAlloc += strlen(args[i])+1;
    toSend[szToAlloc-1] = SEPARATOR;
    
  }
  return toSend;
}



int
main (int argc, char const *argv[])
{
  int sock = 0;
  struct sockaddr_in serv_addr;
  char rxBuf[BUFFER_SIZE] = {0};
  char *cmd = NULL;
  //Les commandes sont créées avec 4 arguments max
  char  arg1[CMD_BUFFER_SIZE], 
        arg2[CMD_BUFFER_SIZE], 
        arg3[CMD_BUFFER_SIZE], 
        arg4[CMD_BUFFER_SIZE],
        arg5[CMD_BUFFER_SIZE];
  char* args[5] = {arg1,arg2,arg3,arg4,arg5};
  char trameHeader = 0, nbArg = 0;
  char *toSend = NULL;


  if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf (stderr,"Impossible d'ouvrir le socket");
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
      return EXIT_FAILURE;
    }

  // while (1)
  //   {
      recv (sock, rxBuf, BUFFER_SIZE,0);
      printf ("%s\n", rxBuf);
      bzero (rxBuf, BUFFER_SIZE);
      fgets (rxBuf, BUFFER_SIZE, stdin);
      nbArg = sscanf (rxBuf, "%s %s %s %s %s", arg1, arg2, arg3, arg4,arg5);
      
     trameHeader = getOperationCode (arg1);
     if(nbArg < 2)
        trameHeader |= KO;
      if (trameHeader & KO)
	{
	  fprintf (stderr,"\nCommande non reconnue\n");
	}
      else
	{
    trameHeader |= OK;
	
    toSend = allocateTrame(nbArg,args,trameHeader);

    printf ("\nCommande %s envoyée\n", cmds[toSend[0] & 0x7]);
      
    send (sock, toSend, strlen (toSend) , 0);
    //
    fflush (stdout);
    bzero(rxBuf,BUFFER_SIZE);
    free (toSend);
	}
  printf("\nRentrez une nouvelle commande :\n");
    // }


  return 1;
}
