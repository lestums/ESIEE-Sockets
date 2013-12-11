#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#define h_addr h_addr_list[0]

int main(int argc, char* argv[])
{
	
			/*socket du client*/
			int d_s, res;
			struct sockaddr_in myAdd;

			d_s = socket(AF_INET, SOCK_STREAM, 0);
			if(d_s == -1)
			{
					printf("Erreur de création de la socket!\n");
					perror("L'erreur est la suivante ");
					return 0;
			}

			myAdd.sin_family = AF_INET;
			myAdd.sin_port = htons(9000);
			myAdd.sin_addr.s_addr = INADDR_ANY;
			memset(myAdd.sin_zero, 0, 8);

	

			res = bind(d_s, (struct sockaddr*) &myAdd, sizeof(myAdd));
			if(res == -1)
			{
					printf("Erreur lors du bind!\n");
					return 0;
			}
			

			/*socket du serveur*/
			struct sockaddr_in serveur;
			struct hostent* h_s;
	
			h_s = gethostbyname(argv[1]);
			if(h_s == NULL)
			{
					printf("Erreur avec le résultat de gethostbyname()\n");
			}
			serveur.sin_family = AF_INET;
			memcpy(&serveur.sin_addr.s_addr, h_s->h_addr, 4);
			serveur.sin_port = htons(atoi(argv[2]));

			if(connect(d_s, (struct sockaddr*) &serveur, sizeof(serveur)) == -1)
			{
				printf("Echec connexion !");
			}
			
			char requete1[100]="GET /";
			char* requete2=argv[3];
			char requete3[100]=" HTTP/1.1\nHost: ";
			char* requete4=argv[1];
			char requete5[100]=":";
			char* requete6=argv[2];
			char requete7[1000]="\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:17.0) Gecko/20131029 Firefox/17.0\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\nAccept-Language: en-US,en;q=0.5\nAccept-Encoding: gzip, deflate\nConnection: keep-alive\n\n";
			char requete[1000]="";
			strcat(requete, requete1);
			strcat(requete, requete2);
			strcat(requete, requete3);
			strcat(requete, requete4);
			strcat(requete, requete5);
			strcat(requete, requete6);
			strcat(requete, requete7);
			write(d_s, (void*) requete, sizeof(requete));
			
			char reponse[1000000]="";
			char reponse2[1000000]="";
			char reponse3[1000000]="";
			
			/*read(d_s, (void*) reponse, sizeof(reponse));
			read(d_s, (void*) reponse2, sizeof(reponse2));
			read(d_s, (void*) reponse3, sizeof(reponse3));

			strcat(reponse, reponse2);
			strcat(reponse, reponse3);*/

			int buffer = recv(d_s, (void*) reponse2, sizeof(reponse2), MSG_PEEK);
			buffer=0;
			while(buffer<5)
			{
				read(d_s, (void*) reponse3, sizeof(reponse3));
				strcat(reponse, reponse3);
				//buffer = recv(d_s, (void*) reponse2, sizeof(reponse2), MSG_PEEK);
				buffer++;
			}
			close(d_s);

			printf("Tout marche!!! et la reponse est \n%s\n%d\n",reponse,buffer);

			return 0;
}
