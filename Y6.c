#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#define SOCK_PATH "socket"
#define BUF_SIZ 1024
#include <sys/wait.h>


void fermerSocket(int socket){
 close(socket);
}

int ouvrirSocketServeur()
{
    int s,len;
    struct sockaddr_un local;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        printf("socket unsuccessfully%s\n", strerror(errno));
    }
    else
    {
        printf("socket successful\n");
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1)
    {
        printf("bind unsuccessfully%s\n", strerror(errno));
    }
    else
    {
        printf("bind successful\n");
    }

    if (listen(s, 5) == -1)
    {
        printf("listen unsuccessfully%s\n", strerror(errno));
    }
    else
    {
        printf("listen successful\n");
    }
    return s;
}

int accepterConnexion(int socket)
{
    struct sockaddr_un newsockaddr;
    int newsockaddr_size = sizeof(struct sockaddr_un);
    int connfd = accept(socket, (struct sockaddr *) &newsockaddr, &newsockaddr_size);
    if(connfd == -1)
    {
        printf("accept unsuccessfully%s\n", strerror(errno));
    }
    else
    {
        printf("accept successful\n");
    }
}

int ecouteClient(int socket, char* str)
{
    int n = recv(socket, str, 100, 0);
    if(n > 0)
    {
        str[n] = '\0';
        printf("Server receive> %s", str);
    }
    else if(n < 0)
    {
         perror("recv");
    }
}

void serveurFactice()
{
    int listenfd, connfd;
    char buf[BUF_SIZ];
    /*gestionAlarmes(); */
    listenfd = ouvrirSocketServeur();
    printf("after socket: %d",listenfd);
    while(1)
    {
        connfd = accepterConnexion(listenfd);
        /*globfd = connfd;*/
        ecouteClient(connfd, buf);
        switch (buf[0])
        {
        case 'R':
            send(connfd, "G", sizeof("G"), 0);
            break;
        case 'K':
            send(connfd, "D", sizeof("D"), 0);
            fermerSocket(connfd);
            exit(0);
            break;
        default:
            fermerSocket(connfd);
            break;
        }
    }
    exit(0);
}
int main(void)
{
    serveurFactice();
    return 0;
}
