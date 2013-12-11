#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{

        int s_ecoute, res;

        struct sockaddr_in myAdd;

        s_ecoute = socket(AF_INET, SOCK_STREAM, 0);
        if(s_ecoute == -1)
        {
                printf("Erreur de création de la socket!\n");
                perror("L'erreur est la suivante ");
                return 0;
        }

        myAdd.sin_family = AF_INET;
        myAdd.sin_port = htons(atoi(argv[1]));
        myAdd.sin_addr.s_addr = INADDR_ANY;

        memset(myAdd.sin_zero, 0, 8);



        res = bind(s_ecoute, (struct sockaddr*) &myAdd, sizeof(myAdd));

        if(res == -1)
        {
                printf("Erreur lors du bind!\n");
                return 0;
        }

        struct sockaddr_in client;

        int taille = sizeof(client);
        char requete[1000];
				/* 1 client en attente de connexion */

				if(listen(s_ecoute,1)==-1)
				{
 								printf("Erreur lors du listen!\n");
                return 0;
				}
	
				int s_service = accept(s_ecoute,(struct sockaddr*) &client, &taille);

				if(s_service==-1)
				{
 								printf("Erreur lors du accept!\n");
                return 0;
				}

				read(s_service, (void*) requete, sizeof(requete));

				write(s_service, (void*) requete, sizeof(requete));
        /*int recev = recvfrom(d_s, requete, 1000, 0,(struct sockaddr*) &client, &taille);
        if(recev == -1)
        {
                perror("ne reçoit rien");
                return 0;
        }*/

        printf("La requete de firefox est  : \n%s", requete);
				close(s_ecoute);		
				close(s_service);

        printf("Tout marche!!!%d\n",getpid());
				
        return 0;
}
