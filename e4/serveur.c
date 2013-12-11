#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{

        int s_ecoute1, s_ecoute2, res1, res2;

        struct sockaddr_in myAdd1, myAdd2;

        s_ecoute1 = socket(AF_INET, SOCK_STREAM, 0);
		  s_ecoute2 = socket(AF_INET, SOCK_STREAM, 0);
        if(s_ecoute1 == -1 || s_ecoute2 == -1)
        {
                printf("Erreur de création de la socket 1!\n");
                perror("L'erreur est la suivante ");
                return 0;
        }

        myAdd1.sin_family = AF_INET;
        myAdd1.sin_port = htons(atoi(argv[1]));
        myAdd1.sin_addr.s_addr = INADDR_ANY;

        memset(myAdd1.sin_zero, 0, 8);

        myAdd2.sin_family = AF_INET;
        myAdd2.sin_port = htons(atoi(argv[2]));
        myAdd2.sin_addr.s_addr = INADDR_ANY;

        memset(myAdd2.sin_zero, 0, 8);



        res1 = bind(s_ecoute1, (struct sockaddr*) &myAdd1, sizeof(myAdd1));
        res2 = bind(s_ecoute2, (struct sockaddr*) &myAdd2, sizeof(myAdd2));

        if(res1 == -1 || res2 == -1)
        {
                printf("Erreur lors du bind!\n");
                return 0;
        }

        struct sockaddr_in client;

        int taille = sizeof(client);
        char requete[1000];
				/* 1 client en attente de connexion */

				if(listen(s_ecoute1,1)==-1)
				{
 								printf("Erreur lors du listen!\n");
                return 0;
				}
	
				int s_service1 = accept(s_ecoute1,(struct sockaddr*) &client, &taille);
				
				pid_t pid = fork();
				if (pid == -1)
				{
					printf ("Erreur fork\n");
					return 0;
			   }	
				if (pid == 0)
				{
					const char * fic = "log_file.txt";
					int fo = open(fic, O_RDWR);
					int r = read(s_service1, (void*) requete, sizeof(requete));

					write(fo,(const void*) requete, sizeof(requete));

					printf("\nLa requete de firefox est  : \n%s", requete);
					close(fo);
					//write(s_service1, (void*) requete, sizeof(requete));
				}
				else{return 0;}
				/*int s_service2 = accept(s_ecoute2,(struct sockaddr*) &client, &taille);

				if(s_service1 == -1 || s_service2 == -1)
				{
 								printf("Erreur lors du accept!\n");
                return 0;
				}

				
        int recev = recvfrom(d_s, requete, 1000, 0,(struct sockaddr*) &client, &taille);
        if(recev == -1)
        {
                perror("ne reçoit rien");
                return 0;
        }

        printf("La requete de firefox est  : \n%s", requete);
				close(s_ecoute1);		
				close(s_service1);

        printf("Tout marche!!!%d\n",getpid());*/
				
        return 0;
}
