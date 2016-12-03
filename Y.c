#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define PORT_NUMBER 7676
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main()
{
    int sockfd, portno, n;
    char buffer[256];
    char currentWorkingDirectory[1024];
    if (getcwd(currentWorkingDirectory, sizeof(currentWorkingDirectory)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", currentWorkingDirectory);
    /*struct sockaddr_in serv_addr, cli_addr;
    struct hostent *server;

    /*-----{ SERVER }-------
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    serv_addr.sin_family = AF_UNIX;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);*/

    return 0;
}
