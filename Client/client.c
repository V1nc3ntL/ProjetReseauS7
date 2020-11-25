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
#include <math.h>
#include "../bankingprotocol.h"
#include "client.h"
#include <arpa/inet.h>

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
char *
allocateTrame (int nbArg, char *args[], char trameHeader, ssize_t * trameSize)
{
  int i = 0, j, k;
  unsigned char tmpChar = 0;
  char *toSend = NULL;
  int tmp = 0;
  float tmpF = 0;
  ssize_t szToAlloc = 1;

  for (i = 1; i < nbArg && i < 4; i++)
    {
      szToAlloc += strlen (args[i]);
    }

  toSend =
    calloc ((szToAlloc +=
	     nbArg) + 2 + sizeof (long), (szToAlloc + nbArg) + sizeof (long));
  toSend[0] = trameHeader;
  toSend[1] = SEPARATOR;
  szToAlloc = 2;

  for (i = 1; i < nbArg; i++)
    {
      if (i < 4)
	{
	  memmove (toSend + szToAlloc, args[i], strlen (args[i]));
	  szToAlloc += strlen (args[i]) + 1;
	  toSend[szToAlloc - 1] = SEPARATOR;
	  *trameSize = szToAlloc;
	}
      else
	{
	  //Détermine si le chiffre est flottant
	  for (j = 0; j < strlen (args[i]); j++)
	    {
	      if (args[i][j] == '.' || args[i][j] == ',')
		{
		  break;
		}
	    }

	  if (j != strlen (args[i]))
	    {
	      tmpF = strtof (args[i], NULL);

	      // L'arrondi évite des problèmes de
	      // virgule flottante
	      tmp = (int) (round (tmpF * 100.00)) / 1.00;
	    }

	  else
	    {
	      tmp = atol (args[i]) * 100;
	    }

	  for (int i = 0, j = 0; i < sizeof (int); i++)
	    {
	      toSend[szToAlloc + i] = (unsigned char) ((tmp >> i * 8) & 0xff);
	    }
	  toSend[szToAlloc + i] = SEPARATOR;

	  toSend[szToAlloc + i + 1] = '\n';
	  *trameSize = szToAlloc + i + 1;
	  break;
	}
    }


  return toSend;
}

int
displayReturnCommand (char *rxBuf, char* account)
{
  int ret = 1;
  char buffer[BUFFER_SIZE];
  int i = 0,j=0, dateI =0,hI =0,mI =0 ,miI = 0, sI = 0, dI = 0, tmpInt = 0,tmp=0,sz = NB_OPERATIONS;

  if (*rxBuf & KO)
    {
      printf ("\nKO : La commande n'a pas été traitée par le serveur\n");
    }
  else
    {

      if(OP_CODE(rxBuf) == RES_OPERATION){
        
        rxBuf = rxBuf+1;

        for(j = NB_OPERATIONS*RET_RES_OP_SZ-RET_RES_OP_SZ ; j >= 0 ; j-= RET_RES_OP_SZ){
                 i=0;
                 // On a plus d'opérations à afficher

             //Décalage de la date
          for(tmpInt = 0; i < sizeof(int) ; i ++)
            tmpInt  += (unsigned int) ((((unsigned char) rxBuf[j+i])) << ((i)*8));

            sprintf(buffer,"%d",tmpInt);
            sscanf(buffer,"%04d%02d%02d",&dateI,&mI,&dI);
           // Permet d'afficher le
            if(!dI){
              if(j == 0){
                printf("\nAucune opérations sur ce compte.");
              }else{
                sz--;
              }
            }
            else{
              if(sz){
                printf ("\n%d dernieres operations sur %s : \n",sz,account);
                sz = 0;
              }
              printf("\tLe %d / %d / %d  à ",dI,mI ,dateI);
            
            //Décalage avec séparateur
            tmp = i+1;
                        //Sauvegarde de l'indice
            for(tmpInt = 0 ,i = 0; i < sizeof(int) ; i ++)
                tmpInt  +=(unsigned int) ((((unsigned char) rxBuf[j+tmp+i])) << ((i)*8));

            sprintf(buffer,"%d",tmpInt);
            
            // Si la valeur récupérée est < 10000 il y a 5 chiffres
            if(tmpInt >= 100000 )
              sscanf(buffer,"%02d%02d%02d",&hI,&miI,&sI);
            else
              sscanf(buffer,"%01d%02d%02d",&hI,&miI,&sI);
            
            printf("%02d:%02d:%02d :\n",hI,miI ,sI); 
            
            tmp += i+1;
            
            for(tmpInt = 0 ,i = 0; i < sizeof(int) ; i ++)
                  tmpInt  += (unsigned int) (((unsigned char) rxBuf[j+tmp+i])) << (i*8);  

            tmp += i+1; 
      
      switch(rxBuf[tmp+j]){
            case 'R':
              hI = 1;
              break;
            case 'A':
              hI =0;
             break; 
            default:
              break;
          }
               printf("\t\t%s\t:",cmds[hI]);
               printf("\t%.2f\n",tmpInt/100.00);
               hI =0;
        }
      }
   
      }
      else{
        if(*rxBuf & RES_BAL){

      for(tmpInt = 0 ,i = 0; i < sizeof(int) ; i ++)
          tmpInt  += (unsigned int) (((unsigned char) rxBuf[i+1])) << (i*8);  
        
        printf("\nSolde du compte \t%s\t : \t %.2f\n",account,tmpInt/100.00);
        }else{
            printf ("\nOK : La commande %s a été traitée par le serveur\n",
	          cmds[OP_CODE (rxBuf)]);
        }

      }
    }
    
}



int
main (int argc, char const *argv[])
{
  int sock = 0;
  struct sockaddr_in serv_addr;
  char rxBuf[BUFFER_SIZE] = { 0 };
  char *cmd = NULL;
  ssize_t trameSize = 0;
  //Les commandes sont créées avec 4 arguments max
  char arg1[CMD_BUFFER_SIZE],
    arg2[CMD_BUFFER_SIZE],
    arg3[CMD_BUFFER_SIZE], arg4[CMD_BUFFER_SIZE], arg5[CMD_BUFFER_SIZE];
  char *args[5] = { arg1, arg2, arg3, arg4, arg5 };
  char trameHeader = 0, nbArg = 0;
  char *toSend = NULL;


  if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf (stderr, "Impossible d'ouvrir le socket");
      return -1;
    }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons (PORT);
  //Connecte à local host, on doit récupérer
  // L'IP dans une chaîne 
  // Convert IPv4 and IPv6 addresses from text to binary form 
  if (inet_pton (AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
      fprintf (stderr, "\nInvalid address/ Address not supported \n");
      return -1;
    }


  // Mode connecté
  if (connect (sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
    {
      fprintf (stderr, "\nConnection Failed \n");
      return EXIT_FAILURE;
    }

  // while (1)
  //   {
  recv (sock, rxBuf, BUFFER_SIZE, 0);
  printf ("%s\n", rxBuf);
  bzero (rxBuf, BUFFER_SIZE);
  fgets (rxBuf, BUFFER_SIZE, stdin);
  nbArg = sscanf (rxBuf, "%s %s %s %s %s", arg1, arg2, arg3, arg4, arg5);

  trameHeader = getOperationCode (arg1);
  if (nbArg < 2)
    trameHeader |= KO;
  if (trameHeader & KO)
    {
      fprintf (stderr, "\nCommande non reconnue\n");
    }
  else
    {
      trameHeader |= OK;

      // Création à envoyer par le client
      toSend = allocateTrame (nbArg, args, trameHeader, &trameSize);

      send (sock, toSend, trameSize, 0);

      bzero (rxBuf, BUFFER_SIZE);
      free (toSend);
      // Attente du retour du serveur
      recv (sock, rxBuf, BUFFER_SIZE, 0);
      displayReturnCommand (rxBuf, arg3);
    }

  return 1;
}
