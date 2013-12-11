#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{

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
        myAdd.sin_port = htons(atoi(argv[1]));
        myAdd.sin_addr.s_addr = INADDR_ANY;

        memset(myAdd.sin_zero, 0, 8);



        res = bind(d_s, (struct sockaddr*) &myAdd, sizeof(myAdd));

        if(res == -1)
        {
                printf("Erreur lors du bind!\n");
                return 0;
        }

        struct sockaddr_in client;

        int taille = sizeof(client);
        char mess[10000];
        char mess2[100];

        int recev = recvfrom(d_s, mess, 10000, 0,(struct sockaddr*) &client, &taille);
        recev = recvfrom(d_s, mess2, 100, 0,(struct sockaddr*) &client, &taille);
        if(recev == -1)
        {
                perror("ne reçoit rien");
                return 0;
        }

				

        printf("message : %s\n et PID : %s\n", mess, mess2);

        // doit retourner le PID et la chaine reçu
        int send_message = sendto(d_s, (void *) mess, sizeof(mess), 0, (struct sockaddr*) &client, sizeof(client));

				int pid = getpid();
				char pid_s[5]="";
				sprintf(pid_s,"%d",pid);
				send_message = sendto(d_s, (void *) pid_s, sizeof(pid_s), 0, (struct sockaddr*) &client, sizeof(client));

        printf("Tout marche!!!%d\n",getpid());
        return 0;
}
