#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

#define h_addr h_addr_list[0]

int main(int argc, char* argv[])
{
	
	/*socket du client*/
	int d_s, res;
	struct sockaddr_in myAdd;

	d_s = socket(AF_INET, SOCK_DGRAM, 0);
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

	int pid = getpid();
	char pid_s[5]="";
	sprintf(pid_s,"%d",pid);

	int send_message = sendto(d_s, (void *) argv[3], sizeof(argv[3]), 0, (struct sockaddr*) &serveur, sizeof(serveur));
	send_message = sendto(d_s, (void *) pid_s, sizeof(pid_s), 0, (struct sockaddr*) &serveur, sizeof(serveur));

	int taille = sizeof(serveur);
	char mess[10000];
	char mess2[100];
	int recev = recvfrom(d_s, mess, 10000, 0,(struct sockaddr*) &serveur, &taille);
	recev = recvfrom(d_s, mess2, 100, 0,(struct sockaddr*) &serveur, &taille);
	printf("message : %s\n et PID : %s\n", mess, mess2);

	printf("Tout marche!!!%s\n",pid_s);

	return 0;
}
