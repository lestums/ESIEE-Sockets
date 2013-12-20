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

	sprintf(requete,"%s%s%s%s%s%s%s","GET /",argv[3]," HTTP/1.1\nHost: ",argv[1],":",argv[2],"\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:17.0) Gecko/20131029 Firefox/17.0\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\nAccept-Language: en-US,en;q=0.5\nAccept-Encoding: gzip, deflate\nConnection: Keep-Alive\n\n");	

	send(lSocket, (void*) requete, sizeof(requete),0);

	byteCount = recv(lSocket, (void*) reponse, sizeof(reponse),0);
	printf("Reponse du serveur : \n%s\n",reponse);
	close(lSocket);
	return 0;
}
