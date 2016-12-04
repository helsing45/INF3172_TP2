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

void serveurFactice()
{

    int listenfd, connfd;
    char buf[BUF_SIZ];
    /*gestionAlarmes();*/
    struct sockaddr_un X;
    /*listenfd = ouvrirSocketServeur();*/
    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        printf("Socket fail: %s\n", strerror(errno));
    }
    else
    {
        printf("Socket successful\n");
    }
    X.sun_family = AF_UNIX;
    strcpy(X.sun_path, SOCK_PATH);
    unlink(X.sun_path);
    int len = strlen(X.sun_path) + sizeof(X.sun_family);
    if (bind(listenfd, (struct sockaddr *)&X, len) == -1)
    {
        printf("bind fail: %s\n", strerror(errno));
    }
    else
    {
        printf("bind successful\n");
    }

    if (listen(listenfd, 5) == -1)
    {
        printf("listen fail: %s\n", strerror(errno));
    }
    else
    {
        printf("listen successful\n");
    }

    while(1)
    {
        /*connfd = accepterConnexion(listenfd);*/
        int Xsize = sizeof(X);
        if ((connfd = accept(listenfd, (struct sockaddr *)&X, &Xsize)) == -1)
        {
            printf("accept fail: %s\n", strerror(errno));
        }
        else
        {
            printf("accept successful\n");
        }

        /*globfd = connfd;*/
        /*ecouteClient(connfd, buf);*/
        int t;
        if ((t=recv(connfd, buf, 100, 0)) > 0)
        {
            buf[t] = '\0';
            printf("echo> %s", buf);
        }
        else
        {
            if (t < 0) perror("recv");
            else printf("Server closed connection\n");
            exit(1);
        }
        switch (buf[0])
        {
        case 'R':
            send(connfd, "G", sizeof("G"), 0);
            break;
        case 'K':
            send(connfd, "D", sizeof("D"), 0);
            close(connfd);
            exit(0);
            break;
        default:
            close(connfd);
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
