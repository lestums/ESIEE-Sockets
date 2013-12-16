#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>

int main(int argc, char* argv[]){

        struct sockaddr_in serverAdd;
	struct sockaddr_in clientAdd;
	
        int lSocket, lService, res;
	int taille = sizeof(clientAdd);
        char requete[2000];
	char reponse[7] = "OK HTTP";
	
        lSocket = socket(AF_INET, SOCK_STREAM, 0);

        if(lSocket == -1){
                perror("Erreur lors de la création de la socket !\n");
                return -1;
        }

        serverAdd.sin_family = AF_INET;
        serverAdd.sin_port = htons(atoi(argv[1]));
        serverAdd.sin_addr.s_addr = INADDR_ANY;
        memset(serverAdd.sin_zero, 0, 8);
        res = bind(lSocket, (struct sockaddr*) &serverAdd, sizeof(serverAdd));

        if(res == -1){
                perror("Erreur lors du bind !\n");
                return -1;
        }

        /* 1 clientAdd en écoute */
	if(listen(lSocket,1) == -1){
 		perror("Erreur lors du listen !\n");
                return -1;
	}
	
	lService = accept(lSocket,(struct sockaddr*) &clientAdd, &taille);

	if(lService==-1){
 		perror("Erreur lors du accept !\n");
                return -1;
	}

	
	read(lService, (void*) requete, sizeof(requete));
	fprintf(stdout,"La requete du navigateur est  : \n---------\n %s \n---------\n", requete);

	write(lService, (void*) reponse, sizeof(reponse));
	
	close(lSocket);		
	close(lService);

        fprintf(stdout,"End of line...\n");
				
        return 0;
}
