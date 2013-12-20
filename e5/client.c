#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#define h_addr h_addr_list[0]

void usage(void){
	fprintf(stderr,"Utilisation : ./client \"nom du serveur a joindre\" \"Port utilisé\" \"Fichier demandé\"");
}

int main(int argc, char* argv[])
{
	if(argc < 4){
		usage();
		return -1;
	}

	struct sockaddr_in clientAdd;
	struct sockaddr_in serverAdd;
	struct hostent* h_s;

	char requete[1000];
	char reponse[65536];
	char* message;

	int lSocket, result;
	int byteCount;
	
	/*Socket du client*/
	lSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(lSocket == -1){
		perror("Erreur lors de la création de la socket !\n");
		return 0;
	}

	clientAdd.sin_family = AF_INET;
	clientAdd.sin_port = htons(9000);
	clientAdd.sin_addr.s_addr = INADDR_ANY;
	memset(clientAdd.sin_zero, 0, 8);

	result = bind(lSocket, (struct sockaddr*) &clientAdd, sizeof(clientAdd));
	if(result == -1){
		perror("Erreur lors du bind!\n");
		return -1;
	}
		

	/*Adresse du serverAdd distant à joindre*/
	h_s = gethostbyname(argv[1]);
	if(h_s == NULL){
		perror("Erreur avec le résultat de gethostbyname()\n");
		return -1;
	}
	
	serverAdd.sin_family = AF_INET;
	memcpy(&serverAdd.sin_addr.s_addr, h_s->h_addr, 4);
	serverAdd.sin_port = htons(atoi(argv[2]));

	if(connect(lSocket, (struct sockaddr*) &serverAdd, sizeof(serverAdd)) == -1){
		perror("Echec lors de la connexion !");
		return -1;
	}

	sprintf(requete,"%s%s%s%s%s%s%s","GET /",argv[3]," HTTP/1.1\nHost: ",argv[1],":",argv[2],"\nProxy-connection: Keep-Alive\n\n");	
	
	send(lSocket, (void*) requete, sizeof(requete),0);
	message = (char*) malloc(sizeof(char)*512);	
	while(byteCount = recv(lSocket, (void*) reponse, sizeof(reponse),0)){
		if(strlen(reponse) + strlen(message) > sizeof(message)){
			message = realloc(message, (strlen(reponse) + strlen(message))*512*sizeof(char));		
		}		
		sprintf(message,"%s%s",message,reponse);	
	}
	printf("Reponse du serveur : \n%s\n",message);
	close(lSocket);
	return 0;
}
