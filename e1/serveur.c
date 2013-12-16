#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>

#define MESSAGE_SIZE 500

void usage(void){
	fprintf(stderr,"Utilisation : ./serveur \"Port utilisé\"\n");
}

int main(int argc, char** argv){

	if(argc < 2){
		usage();
		return -1;
	}

	struct sockaddr_in serverAdd;
     	struct sockaddr_in clientAdd;

        int sSocket;
  	int taille = sizeof(clientAdd);
	
	char pid[5];
        char message1[MESSAGE_SIZE];
        char message2[MESSAGE_SIZE];
	
        
	/*Création de la socket d'écoute*/
        sSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(sSocket == -1){
                perror("Erreur de création de la socket : N°"+ errno);
                return 0;
        }

        serverAdd.sin_family = AF_INET;
        serverAdd.sin_port = htons(atoi(argv[1]));
        serverAdd.sin_addr.s_addr = INADDR_ANY;
        memset(serverAdd.sin_zero, 0, 8);

        if(bind(sSocket, (struct sockaddr*) &serverAdd, sizeof(serverAdd)) == -1) {
                perror("Erreur lors du bind !\n");
                return 0;
        }

	fprintf(stdout,"-------------\n");
	fprintf(stdout," Server Side \n");
	fprintf(stdout,"-------------\n");

	/*Reception des informations du client*/
	fprintf(stdout,"Recieved :\n");
        recvfrom(sSocket, message1, MESSAGE_SIZE, 0,(struct sockaddr*) &clientAdd, &taille);
        recvfrom(sSocket, message2, MESSAGE_SIZE, 0,(struct sockaddr*) &clientAdd, &taille);
        fprintf(stdout,"\tMessage : %s\n\tClient PID : %s\n", message1, message2);
	
	sprintf(pid,"%d",getpid());

	/*Envoi des informations au client*/
	fprintf(stdout,"Sent :\n");
	sprintf(message1,"%s%s",message1," (ACK from server)");
        sendto(sSocket, (void *) message1, sizeof(message1), 0, (struct sockaddr*) &clientAdd, sizeof(clientAdd));
	sendto(sSocket, (void *) pid, sizeof(pid), 0, (struct sockaddr*) &clientAdd, sizeof(clientAdd));			
	fprintf(stdout,"\tMessage : %s\n",message1);
	fprintf(stdout,"\tServer PID : %s\n",pid);

        printf("End of line...\n");
        return 0;
}
