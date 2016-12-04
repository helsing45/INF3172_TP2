#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_LENGTH 1024
#define SOCKET_PATH "cq.socket"

/******************************************************/
/**                                                  **/
/**                --- { UTILS } ---                 **/
/**                                                  **/
/******************************************************/
void removeFirst(int argc, char *argv[])
{
    int compteur = 0;
    for (compteur = 0 ; compteur < argc ; compteur++)
    {
        argv[compteur] = argv[compteur + 1];
    }
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int getSocket()
{
    unlink(SOCKET_PATH);/* On enleve l'ancien si le programme n"a pas bien ete fermer*/
    int socketID;
    /* --- CREATE SOCKET --- */
    struct sockaddr_un socket_address;
    if ((socketID = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        printf("socket unsuccessful %s\n", strerror(errno));
    }
    socket_address.sun_family = AF_UNIX;
    strcpy(socket_address.sun_path, SOCKET_PATH);
    int length = strlen(socket_address.sun_path) + sizeof(socket_address.sun_family);

    /* --- BIND SOCKET --- */

    if (bind(socketID, (struct sockaddr *)&socket_address, length) == -1)
    {
        printf("bind unsuccessful%s\n", strerror(errno));
    }

    /* --- LISTEN SOCKET --- */
    if (listen(socketID, 5) == -1)
    {
        printf("listen unsuccessful%s\n", strerror(errno));
    }
    return socketID;

}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int acceptConnectionFor(int socketID)
{
    struct sockaddr_un mockSocketAddress;
    int mockSocketAddressSize = sizeof(mockSocketAddress);
    return accept(socketID, (struct sockaddr *) &mockSocketAddress, &mockSocketAddressSize);

}

/*************************************************/
/*                   OPTION -X                   */
/*************************************************/
void basicExecution(int argc,char *argv[])
{
    /*Enleve la commande*/
    char* commande = argv[0];
    removeFirst(argc, argv);
    argc --;

    int waitProcessId;
    int processId = fork();
    int status;

    if ( processId == 0 )
    {
        int compteur;
        char *execArgs[argc + 2];
        execArgs[0]=commande;
        for (compteur = 0 ; compteur < argc ; compteur++)
        {
            execArgs[compteur + 1] = argv[compteur];
        }
        execArgs[compteur + 1] = NULL;
        int e = execvp(commande, execArgs);
         printf("execvp ID: %d\n",e);

    }
    do
    {
        if (waitpid(processId, &status, WUNTRACED | WCONTINUED) == -1)
        {
            printf("waitpid unsuccessful%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    while (!WIFEXITED(status) && !WIFSIGNALED(status));

}

/*************************************************/
/*                   OPTION -C                   */
/*************************************************/
void basicClientOption(int argc,char *argv[]){
    printf("Cette fonctionnalite n'est pas encore implementer\n");
}

/*************************************************/
/*                   OPTION -Y                   */
/*************************************************/
void dummyServer()
{
    int serverID, connectionID;
    char buffer[BUFFER_LENGTH];
    /*********************************************/
    /*          CREATE NEW SOCKET                */
    /*********************************************/
    serverID = getSocket();
    int done = 0;
    while(!done)
    {
        connectionID = acceptConnectionFor(serverID);
        int n = recv(connectionID, buffer, 100, 0);
        if(n > 0)
        {
            buffer[n] = '\0';
            if(buffer[0] == 'R')
            {
                send(connectionID, "G", sizeof("G"), 0);
            }
            else if(buffer[0] == 'K')
            {
                send(connectionID, "D", sizeof("D"), 0);
                close(connectionID);
                done = 1;
            }
            else
            {
                close(connectionID);
            }
        }
    }
}

/*************************************************/
/*                   OPTION -D                   */
/*************************************************/
void demonServer(){
    printf("Cette fonctionnalite n'est pas encore implementer\n");
}

/*************************************************/
/*                   OPTION -K                   */
/*************************************************/
void terminator(){
    printf("Cette fonctionnalite n'est pas encore implementer\n");
}

/*************************************************/
/*                   OPTION -S                   */
/*************************************************/
void basicServer(){
    printf("Cette fonctionnalite n'est pas encore implementer\n");
}

/*************************************************/
/*                   ULTIMATE                    */
/*                    CLIENT                     */
/*************************************************/
void ultimateClient(int argc, char *argv[]){
    printf("Cette fonctionnalite n'est pas encore implementer\n");
}

int main(int argc, char *argv[])
{
    /*Enleve le nom de la méthode*/
    removeFirst(argc, argv);
    argc --;

    /*Lire la premiere option l'option*/
    char* option = argv[0];

    /* Check if is ultimateClient*/
    if(option[0] != '-'){
        ultimateClient(argc,argv);
    }else if(strcmp("-x", option) == 0)
    {
        removeFirst(argc, argv);
        argc --;
        basicExecution(argc,argv);
    }
    else if(strcmp("-c", option) == 0)
    {
        removeFirst(argc, argv);
        argc --;
        basicClientOption(argc,argv);
    }
    else if(strcmp("-y", option) == 0)
    {
        dummyServer();
    }
    else if(strcmp("-d", option) == 0)
    {
        demonServer();
    }
    else if(strcmp("-k", option) == 0)
    {
        terminator();
    }
    else if(strcmp("-s", option) == 0)
    {
        basicServer();
    }

    return 0;
}
