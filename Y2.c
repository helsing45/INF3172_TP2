#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <string.h>
#define PORT_NUMBER 7676
#define BUFFER_LENGTH 256
#define LISTEN_BACKLOG 10

/******************************************/
/**               SERVEUR                **/
/******************************************/
void initialisationServeur()
{
    /***********************************************************************/
    /* Variable and structure definitions.                                 */
    /***********************************************************************/
    int    s = -1, s2 = -1, bytes;
    /*****************************************/
    /*  ---{ CREATION DU PATH DU SOCKET}---  */
    /* 1) on trouve notre repertoire courant */
    /* 2) creation du nom du socket.         */
    /* 3) on concatene les deux              */
    /*****************************************/
    char SERVER_PATH[1024];
    char CURRENT_WORKING_DIRECTORY[1024];
    char UNIX_SOCKET_NAME[11]="/cp.socket";
    getcwd(CURRENT_WORKING_DIRECTORY, sizeof(CURRENT_WORKING_DIRECTORY));

    char *result = malloc(strlen(CURRENT_WORKING_DIRECTORY)+strlen(UNIX_SOCKET_NAME)+1);
    strcpy(SERVER_PATH, CURRENT_WORKING_DIRECTORY);
    strcat(SERVER_PATH, UNIX_SOCKET_NAME);

    printf("SOCKET UNIX NAME: %s\n",SERVER_PATH);

    /* Au cas ou la derniere fois elle l' application ne c'est pas bien fermer.*/
    unlink(SERVER_PATH);

    char buffer[BUFFER_LENGTH];
    struct sockaddr_un serveraddr,serverClient;

    /*************************************************************************/
    /* The socket() function returns a socket descriptor, which represents   */
    /* an endpoint.  The statement also identifies that the UNIX             */
    /* address family with the stream transport (SOCK_STREAM) will be        */
    /* used for this socket.                                                 */
    /*************************************************************************/
    s = socket(AF_UNIX, SOCK_STREAM, 0);
    if(s < 0)
    {
        perror("socket() failed");
    }
    bzero((char *) &serveraddr, sizeof(serveraddr));
    /********************************************************************/
    /* After the socket descriptor is created, a bind() function gets a */
    /* unique name for the socket.                                      */
    /********************************************************************/
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SERVER_PATH);
    int len = strlen(serveraddr.sun_path) + sizeof(serveraddr.sun_family);
    if(bind(s, (struct sockaddr *)&serveraddr, len) < 0)
    {
        perror("bind() failed");
    }
    /********************************************************************/
    /* The listen() function allows the server to accept incoming       */
    /* client connections.  In this example, the backlog is set to 10.  */
    /* This means that the system will queue 10 incoming connection     */
    /* requests before the system starts rejecting the incoming         */
    /* requests.                                                        */
    /********************************************************************/
    if (listen(s, LISTEN_BACKLOG) < 0)
    {
        perror("listen() failed");
    }

    printf("Ready for client connect().\n");
    int clilen = sizeof(serverClient);
    while (1)
    {
        printf("in the loop\n");
        s2 = accept(s, (struct sockaddr *) &serverClient, &clilen);
        if(s2 < 0)
        {
            printf("in the do loop accepted fail\n");
            perror("accept() failed");
            break;
        }
        bytes = recv(s2,buffer,sizeof(buffer),0);
        if(bytes < 0)
        {
            perror("recv() failed");
            break;
        }
        char *RECEIVED_DATA;
        buffer[bytes] = '\0';
        strcpy(RECEIVED_DATA, buffer);
        printf("%s received \n",RECEIVED_DATA);
        /********************************************************************/
        /* Echo the data back to the client                                 */
        /********************************************************************/
        bytes = send(s2, buffer, sizeof(buffer), 0);
        if (bytes < 0)
        {
            perror("send() failed");
            break;
        }
    } /* end of while */

    unlink(SERVER_PATH);
    if(s != -1)close(s);
}

int main()
{
    initialisationServeur();
    return 0;
}
