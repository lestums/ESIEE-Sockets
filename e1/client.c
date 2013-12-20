#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

#define h_addr h_addr_list[0]
#define MESSAGE_SIZE 500

void usage(void){
	fprintf(stderr,"Utilisation : ./client \"nom du serveur\" \"Port utilisé\" \"Chaine à envoyer\"\n");
}

int main(int argc, char** argv){

	if(argc < 4){
		usage();
		return -1;
	}
	struct sockaddr_in localAdd;
	struct sockaddr_in serverAdd;
	struct hostent* h_s;

	int cSocket,result;
	int size = sizeof(serverAdd);

	char pid[5];
	char message1[MESSAGE_SIZE];
	char message2[MESSAGE_SIZE];

	/*Socket du client*/
	cSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(cSocket == -1){
		perror("Erreur lors de la création de la socket");
		return 0;
	}

	localAdd.sin_family = AF_INET;
	localAdd.sin_port = htons(5600);
	localAdd.sin_addr.s_addr = INADDR_ANY;
	memset(localAdd.sin_zero, 0, 8);

	result = bind(cSocket, (struct sockaddr*) &localAdd, sizeof(localAdd));

	if(result == -1){
		perror("Erreur lors du bind!\n");
		return 0;
	}	
	
	/*Adresse du serveur distant à joindre*/
	h_s = gethostbyname(argv[1]);
	if(h_s == NULL){
		perror("Erreur : gethostbyname()\n");
		return 0;
	}
	serverAdd.sin_family = AF_INET;
	serverAdd.sin_port = htons(atoi(argv[2]));
	memcpy(&serverAdd.sin_addr.s_addr, h_s->h_addr, 4);

	fprintf(stdout,"-------------\n");
	fprintf(stdout," Client Side \n");
	fprintf(stdout,"-------------\n");
	sprintf(pid,"%d",getpid());
	strcpy(message1,argv[3]);

	/*Envoi des données au serveur*/
	fprintf(stdout,"Sent :\n");
	sendto(cSocket, (void *) message1, sizeof(message1), 0, (struct sockaddr*) &serverAdd, sizeof(serverAdd));
	sendto(cSocket, (void *) pid, sizeof(pid), 0, (struct sockaddr*) &serverAdd, sizeof(serverAdd));
  	fprintf(stdout,"\tMessage : %s\n",argv[3]); 
	fprintf(stdout,"\tClient PID : %s\n",pid); 
	
	/*Reception de la réponse*/
	fprintf(stdout,"Recieved :\n");
	recvfrom(cSocket, message1, MESSAGE_SIZE, 0,(struct sockaddr*) &serverAdd, &size);
	recvfrom(cSocket, message2, MESSAGE_SIZE, 0,(struct sockaddr*) &serverAdd, &size);
	printf("\tMessage : %s\n\tServer PID : %s\n", message1, message2);
	
	printf("End of line...\n");
        return 0;
}
