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

void* treatConnexion(void* socket){

	static const char welcomeMsg[]= WELCOME_CLIENT_MESSAGE; 
		char buffer[1024] = {0};
	write(*(int*)socket,welcomeMsg,sizeof(welcomeMsg));



}


int main(int argc, char const *argv[]) 
{ 
	int server, clientSock, valread, opt = 1,i;
	struct sockaddr_in address; 
	int addrlen = sizeof(address);
	customerArray custs;

	pthread_t thread_id;
	
	if(argc == 4){
		getClientsAndAccountFrom(argv[2],argv[3],&custs );
	}
	else
		getClientsAndAccountFrom(DEBUG_CLIENTF,DEBUG_ACCOUNTF,&custs );
	

	liberateCustomerArray(custs.c,custs.nbCustomers);




	printf("Launching banking server on port 8888");
	
	// Creation du socket TCP IPv4
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("Probleme dans la creation du socket"); 
		exit(EXIT_FAILURE); 
	} 

	
	// Indique que le socket peut être connecté plusieurs fois
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Lie le serveur à l'adresse PORT
	if (bind(server, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// On fixe le nombre maximum de connexion 
	if (listen(server, NB_MAX_CO 10) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

    while(clientSock = accept(server, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))
		{
		 if( pthread_create( &thread_id , NULL ,  treatConnexion , (void*) &clientSock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
	   // valread = read( new_socket , buffer, 1024); 
	  
	   // send(new_socket , hello , strlen(hello) , 0 ); 
	   // printf("Hello message sent\n"); 
    }
	return 0; 
} 

