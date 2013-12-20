#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>

#define TAILLE_MAX 256

#define STDIN_FS 0

void usage(void){
	fprintf(stderr,"Utilisation : ./serveur \"Port HTTP\" \"Port LOG\"\n");
}

int main(int argc, char* argv[])
{
	if(argc <3){
		usage();
		return -1;
	}
	fd_set rdfs;
	FILE* logfile; 
	FILE* page;
	
    	time_t timestamp;

	struct sockaddr_in httpAdd, logAdd, clientAdd;

        int httpSocket, logSocket;
	int tailleHttp = sizeof(httpAdd);
	int tailleLog = sizeof(logAdd);
	int tailleClient = sizeof(clientAdd);
	int res = 0;
	
	char dbuffer[256] ="";
	char buffer[TAILLE_MAX];
	char reponse[1024] ="";
	char* log = (char*)malloc(sizeof(char)*65536);
	char requete[1000] ="";
	char ipbuffer[256] ="";

	httpSocket = socket(AF_INET, SOCK_STREAM,0);
	logSocket = socket(AF_INET, SOCK_STREAM,0);
        if(httpSocket == -1 || logSocket == -1){
                perror("Erreur lors de la création des sockets !\n");
                return -1;
        }
	

        httpAdd.sin_family = AF_INET;
        httpAdd.sin_port = htons(atoi(argv[1]));
        httpAdd.sin_addr.s_addr = INADDR_ANY;
        memset(httpAdd.sin_zero, 0, 8);

        logAdd.sin_family = AF_INET;
        logAdd.sin_port = htons(atoi(argv[2]));
        logAdd.sin_addr.s_addr = INADDR_ANY;
        memset(logAdd.sin_zero, 0, 8);

	if (bind(logSocket, (struct sockaddr *) &logAdd, sizeof(logAdd)) == -1) {
		perror("Erreur lors du bind (LOG) !");
		return -1;
	}

	if(listen(logSocket,1) == -1){
 		perror("Erreur lors du listen (LOG)!\n");
                return -1;
	}

	if (bind(httpSocket, (struct sockaddr *) &httpAdd, sizeof(httpAdd)) == -1) {
		perror("Erreur lors du bind (HTTP) !");
		return -1;
	}

	if(listen(httpSocket,1) == -1){
 		perror("Erreur lors du listen (HTTP) !\n");
                return -1;
	}
	
	fprintf(stdout,"---------------\n");
	fprintf(stdout,"- HTTP Server -\n");
	fprintf(stdout,"---------------\n");
	
	fprintf(stdout,"\n Serveur en route.\n");
	fprintf(stdout,"\n\tAppuyez sur \"ENTREE\" pour interrompre l'execution\n\n");
	fprintf(stdout,"---------------\n");

	while(1) {
		FD_ZERO(&rdfs);

		/* Ajout du socket pour les requètes HTTP */
		FD_SET(httpSocket, &rdfs);

		/* Ajout du socket pour l'accès aux logs */
		FD_SET(logSocket, &rdfs);

		/* Ajout du descripteur d'entrée standard stdin*/
		FD_SET(STDIN_FS, &rdfs);

		int max = select(FD_SETSIZE, &rdfs, NULL, NULL, NULL);

		if(max == -1){
			perror("Erreur lors du select()");
			return -1;
		} else {
			if(max == 0){
				fprintf(stdout,"Rien \n");
			} else {
				if(FD_ISSET(httpSocket, &rdfs)){
					int httpService = accept(httpSocket,(struct sockaddr*) &clientAdd, &tailleClient);
					if(httpService == -1){
						perror("Erreur lors de accept() (HTTP)");
						return -1;
					} 
					read(httpService, (void*) requete, sizeof(requete));
					if(strstr(requete, "HTTP") == NULL){
						sprintf(reponse, "ERREUR HTTP");
						res = 1;
					} else {
						if(strstr(requete,"page.html") == NULL){
							sprintf(reponse,"HTTP/1.1 404 Not Found\r\nConnection : keep-alive\r\n\r\n");
							sprintf(reponse, "%s<html><body><b><u>404 not found !!!</u></b></body></html>\r\n",reponse);
							res = 2;
						} else {							
							page = fopen("page.html","r");	
							if(page != NULL){
								sprintf(reponse,"HTTP/1.1 200 OK\r\nConnection : keep-alive\r\n\r\n");			
								while(fgets(buffer,TAILLE_MAX,page) != NULL){
									sprintf(reponse,"%s%s",reponse,buffer);					
								}
							} else {
								fprintf(stderr,"Erreur lors de la lecture de la page\n");
								return -1;
							}
							fclose(page);
						}
					}
					write(httpService, (void*) reponse, strlen(reponse));

					timestamp = time(NULL);
					strftime(dbuffer, sizeof(dbuffer), "%A %d %B %Y - %X.", localtime(&timestamp));
					logfile = fopen("log_server.txt","a");
					fprintf(logfile, "Client %s connected on %s => ",inet_ntoa(clientAdd.sin_addr),dbuffer);
	    				switch(res){
						case 0:
							fprintf(logfile,"requested page.html\n");
							break;
						case 1:
							fprintf(logfile, "HTTP ERROR\n");
							break;
						case 2:
							fprintf(logfile, "404\n");
							break;
					}
					res = 0;
					fclose(logfile);
					close(httpService);
	
				/*S'il y a une activité sur le port LOG */			 
				} else if(FD_ISSET(logSocket, &rdfs)){
					int logService = accept(logSocket,(struct sockaddr*) &clientAdd, &tailleClient);
					if(logService == -1){
						perror("Erreur lors de accept() (HTTP)");
						return -1;
					} 	
					logfile = fopen("log_server.txt","r");
					if(logfile != NULL){			
						while(fgets(buffer,TAILLE_MAX,logfile) != NULL){
							strcat(log,buffer);					
						}
					} else {
						fprintf(stderr,"Erreur lors de la lecture du fichier de log !\n");
						return -1;
					}
					fclose(logfile);
					write(logService, (void*) log, strlen(log));
					close(logService);
				} else if(FD_ISSET(STDIN_FS,&rdfs)){
					fprintf(stdout,"Interruption clavier : Terminaison du programme !\n");
					break;				
				}
			}
		}
		memset(buffer, 0, sizeof(buffer));
		memset(log,0,sizeof(log));
		memset(reponse,0,sizeof(reponse));
   	}
	close(httpSocket);
	close(logSocket);		
}
