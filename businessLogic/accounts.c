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

#include "../bankingprotocol.h"
#include "customers.h"
#include "accounts.h"

#define PORT 8080
//Mutex pour gestion des ressources
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
operation *
retrieveOperations (char *opF)
{

}

void
liberateOperation (account * act)
{
  for (int i = 0; i < act->nbOps; i++)
    {
      free (act->ops[i].date);
    }
  free (act->ops);
  act->ops = NULL;
  act->nbOps = 0;
}

//char* opToStr()
int
updateOperationFile (customerArray * custs, char *opF)
{

  FILE *oF = fopen (opF, "a+");
  int ret = 1, i, j, k;
  char tmp[BUFFER_SIZE] = { 0 };

  if (!oF)
    {
      fprintf (stderr, "Impossible d'ouvrir le fichier compte %s", opF);
      ret = EXIT_FAILURE;
    }
  else
    {
      for (i = 0; i < custs->nbCustomers; i++)
	for (j = 0; j < custs->c[i].nbAccount; j++)
	  {
	    for (k = 0; k < custs->c[i].accounts[j].nbOps; k++)
	      {

		fputs (custs->c[i].id, oF);
		fputc (SEPARATOR, oF);
		fputs (custs->c[i].accounts[j].accountId, oF);
		fputc (SEPARATOR, oF);
		fputs (custs->c[i].accounts[j].ops[k].date, oF);
		fputc (SEPARATOR, oF);
		fputs (cmds[custs->c[i].accounts[j].ops[k].type], oF);
		fputc (SEPARATOR, oF);
		bzero (tmp, BUFFER_SIZE);
		sprintf (tmp, "%d\n", custs->c[i].accounts[j].ops[k].amount);
		fputs (tmp, oF);

	      }
	    liberateOperation (&custs->c[i].accounts[j]);
	  }
      fclose (oF);
    }
  return ret;
}

//Créé une trame
char *
createOpeResultsTrame (char *headerOnly)
{

}

int
getLastOperations (char *operationFile, char *id, char *accountId,
		   char **headerOnly)
{
  int ret = 1, i = 0, j = NB_OPERATIONS, k = 0, l=0;
  ssize_t bufS = BUFFER_SIZE;
  int amount = 0;
  char conv[sizeof (int)];
  char tmp[NB_OPERATIONS][BUFFER_SIZE] = { 0 };
  unsigned char buffer[BUFFER_SIZE] = { 0 };
  char *buf = NULL, *account = NULL, *sum = NULL, *date = NULL, *time =
    NULL, *op = NULL, *rest = NULL;

  FILE *opF = fopen (operationFile, "r");
  if (!opF)
    {
      fprintf (stderr, "Impossible d'ouvrir le fichier opération %s", opF);
      ret = EXIT_FAILURE;
    }
  else
    {
      amount = getline (&buf, &bufS, opF);

      while (amount >= 0)
	{
	  strtok_r (buf, SEPARATORSTR, &account);

	  if (!strcmp (id, buf))
	    {
	      strtok_r (account, SEPARATORSTR, &date);

	      if (!strcmp (accountId, account))
		{
		  bzero (buffer, BUFFER_SIZE);

		  strcpy (buffer, date);
		  // Le fichier est dans l'ordre chronologique
		  for (i = NB_OPERATIONS - 1; i > 0; i--)
		    {
		      strcpy (tmp[i], tmp[i - 1]);
		    }

		  j++;
		  strcpy (tmp[0], buffer);
		}
	    }
	  amount = getline (&buf, &bufS, opF);
	}
    

  *headerOnly = (realloc (*headerOnly, RET_RES_OP_SZ * j + j + 1));
  *headerOnly += 1;

  for (i = j, j = 0; j < NB_OPERATIONS && j < i; j++)
    {
      //Découpage de la trame
      strtok_r (tmp[j], SEPARATORSTR, &date);
      strtok_r (date, SEPARATORSTR, &time);
      strtok_r (time, SEPARATORSTR, &op);
      strtok_r (op, SEPARATORSTR, &sum);
      strtok_r (sum, SEPARATORSTR, &rest);
      
      amount = atoi (tmp[j]);
      // Récupération de la date
      for (k = 0,l=0; k < sizeof (int); k++)
           	buffer[k] = (unsigned char) (amount >> (k * 8) & 0xff);
      
      *(buffer + k++) = SEPARATOR;

      //Récupération de l'heure
      amount = atoi (date);
      for(k ; k < (sizeof (int)<<1) +1; k++)
	      buffer[k] = (unsigned char) (amount >>( (k-sizeof(int)-1) * 8) & 0xff);

             
      buffer[k++] = SEPARATOR;
      
        // Récupération du montant
      amount = atoi (op);
      printf(op);

      for (l=0; l < sizeof (int); l++)
	      buffer[k+l] = (unsigned char) ((amount >> l * 8) & 0xff);
      

      //Récupération du type d'opération
      buffer[k + l] = SEPARATOR;
      buffer[k + l+ 1] = time[0] ;
      buffer[k+ l + 2] = SEPARATOR;

      memmove((*headerOnly)+j*RET_RES_OP_SZ,buffer,RET_RES_OP_SZ);
      
      bzero (buffer, BUFFER_SIZE);
      // Découper la date en int
      // Envoyer l'int
      // Le décoder
    }

  // On revient au dévut de la trame
  *headerOnly -= 1;
  (*headerOnly)[j * RET_RES_OP_SZ] = SEPARATOR;
  (*headerOnly)[j * RET_RES_OP_SZ + 1] = '\n';

  **headerOnly |= RES_OPERATION;

  j = 0;
  } 

  //Retourne le nombre d'opérations
  return i;

}

// Met à jour le fichier de compte
int
updateAccountFile (customerArray * custs, char *accountF)
{
  FILE *aF = fopen (accountF, "w");
  int ret = 1, i, j;
  char tmp[BUFFER_SIZE] = { 0 };

  if (!aF)
    {
      fprintf (stderr, "Impossible d'ouvrir le fichier compte %s", accountF);
      ret = EXIT_FAILURE;

    }
  else
    {
      for (i = 0; i < custs->nbCustomers; i++)
	    for (j = 0; j < custs->c[i].nbAccount; j++)
	    {
	      fputs (custs->c[i].id, aF);
	      fputc (SEPARATOR, aF);
	      fputs (custs->c[i].accounts[j].accountId, aF);
	      fputc (SEPARATOR, aF);
	      bzero (tmp, BUFFER_SIZE);
        sprintf (tmp, "%d", custs->c[i].accounts[j].balance);	      
	      fputs (tmp, aF);
	      fputc ('\n', aF);
	    }
	
    }
  fclose (aF);
  
  return ret;
}

// Affiche les opérations d'un compte
void
displayOperations (account * account)
{
  for (int i = 0; i < account->nbOps; i++)
    {
      printf ("\n%s", account->ops[i].date);
      printf (cmds[account->ops[i].type]);
      printf ("%d", account->ops[i].amount);
    }
}

// Ajoute l'opération au compte
void
addOperation (account * act, char opCode, int amount)
{
  time_t t = time (NULL);
  struct tm tm = *localtime (&t);
  char dateStr[DATE_FORMAT_SZ];
  operation *copy = NULL;


  sprintf (dateStr, "%04d%02d%02d:%02d%02d%02d",
	   tm.tm_year + 1900,
	   tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

  if (!act->nbOps)
    {
      act->ops = malloc (sizeof (operation));
      act->ops->date = malloc (DATE_FORMAT_SZ);

      act->ops->amount = amount;
      act->ops->type = opCode;

      memmove (act->ops->date, dateStr, DATE_FORMAT_SZ);
      act->nbOps = 1;
    }
  else
    {
      copy = NULL;
      while (!copy)
	copy =
	  realloc (act->ops,
		   sizeof (operation) * act->nbOps + sizeof (operation));
      act->ops = copy;

      act->ops[act->nbOps].date = malloc (DATE_FORMAT_SZ);
      act->ops[act->nbOps].amount = amount;
      act->ops[act->nbOps].type = opCode;
      memmove (act->ops[act->nbOps].date, dateStr, DATE_FORMAT_SZ);
      act->nbOps++;
    }
}



int
findAccountIndex (customerArray * custs, int customerIndex, char *accountId)
{
  int i;

  for (i = 0; i < custs->c[customerIndex].nbAccount; i++)
    if (!strncmp
	(accountId, custs->c[customerIndex].accounts[i].accountId,
	 strlen (accountId)))
      break;

  if (i == custs->c[customerIndex].nbAccount)
    {
      fprintf (stderr, "\nAucun compte ne correspond\n");
      i = -1;
    }
  return i;
}

//Ajoute une somme sur le compte id
int
addToAccount (customerArray * custs, int customerIndex, int accountIndex,
	      signed int amount)
{

  custs->c[customerIndex].accounts[accountIndex].balance += (int)amount;

  return EXIT_FAILURE;
}



//Ajoute une somme sur le compte id
int
removeFromAccount (customerArray * custs, int customerIndex, int accountIndex,
		   int amount)
{

  custs->c[customerIndex].accounts[accountIndex].balance -= (int) amount;

  return EXIT_SUCCESS;
}
