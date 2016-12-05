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

#define BUFFER_LENGTH 256
#define SOCKET_PATH "cq.socket"
#define MAX_CLIENT 10

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
int createClient(){
    int clientSocket;
    struct sockaddr_un remote;

    /* --- CREATE THE SOCKET --- */
    if ((clientSocket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        printf("socket unsuccessful %s\n", strerror(errno));
        exit(1);
    }

    /* --- CONNECT TO THE SERVER --- */

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCKET_PATH);
    int length = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(clientSocket, (struct sockaddr *)&remote, length) == -1)
    {
        printf("Connect unsuccessful %s\n", strerror(errno));
        exit(1);
    }
    return clientSocket;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
int execute(int argc,char *argv[]){
    /*Enleve la commande*/
    char* commande = argv[0];
    removeFirst(argc, argv);
    argc --;

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
/*************************************************/
/*                   OPTION -X                   */
/*************************************************/
void basicExecution(int argc,char *argv[])
{
    int waitProcessId;
    int processId = fork();
    int status;

    if ( processId == 0 )
    {
        execute(argc,argv);
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
void basicClientOption(int argc,char *argv[])
{
    int clientSocket = createClient();
    char buffer[BUFFER_LENGTH];
    if (send(clientSocket,"R",sizeof("R"),0) == -1)
    {
        printf("Send %s\n", strerror(errno));
        exit(1);
    }
    int t;
    if ((t=recv(clientSocket, buffer, BUFFER_LENGTH, 0)) > 0)
    {
        buffer[t] = '\0';
        char first = buffer[0];
        if(first == 'G')
        {
            execute(argc,argv);
        }
    }
    else
    {
        if (t < 0) perror("recv");
        else printf("Server closed connection\n");
        exit(1);
    }
}

/*************************************************/
/*                   OPTION -Y                   */
/*************************************************/
void dummyServer()
{
    int socketID,connectionSocket;
    struct sockaddr_un local, remote;
    char buffer[BUFFER_LENGTH];

    if ((socketID = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        printf("socket unsuccessful %s\n", strerror(errno));
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCKET_PATH);
    unlink(local.sun_path);
    int length = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(socketID, (struct sockaddr *)&local, length) == -1)
    {
        printf("bind unsuccessful %s\n", strerror(errno));
        exit(1);
    }

    if (listen(socketID, MAX_CLIENT) == -1)
    {
        printf("listen unsuccessful %s\n", strerror(errno));
        exit(1);
    }

    for(;;)
    {
        int done, n;
        int t = sizeof(remote);
        if ((connectionSocket = accept(socketID, (struct sockaddr *)&remote, &t)) == -1)
        {
            printf("accept unsuccessful %s\n", strerror(errno));
            exit(1);
        }

        done = 0;
        do
        {
            int n = recv(connectionSocket, buffer, BUFFER_LENGTH, 0);
            if (n <= 0)
            {
                if (n < 0) printf("recv unsuccessful %s\n", strerror(errno));
                done = 1;
            }
            else
            {
                buffer[n] = '\0';
                char first = buffer[0];
                if(first == 'R')
                {
                    send(connectionSocket,"G",sizeof("G"),0);
                }
                else if(first =='K')
                {
                    send(connectionSocket,"D",sizeof("D"),0);
                    close(connectionSocket);
                    unlink(SOCKET_PATH);
                    exit(0);
                }
                else
                {
                    printf("commande inconnu:  %s\n", buffer);
                    close(connectionSocket);
                    exit(0);
                }
            }
        }
        while (!done);

        close(connectionSocket);
    }
}

/*************************************************/
/*                   OPTION -D                   */
/*************************************************/
void demonServer()
{
    printf("Cette fonctionnalite n'est pas encore implementer\n");
}

/*************************************************/
/*                   OPTION -K                   */
/*************************************************/
void terminator()
{
    int clientSocket = createClient();
    char buffer[BUFFER_LENGTH];
    if (send(clientSocket,"K",sizeof("K"),0) == -1)
    {
        printf("Send %s\n", strerror(errno));
        exit(1);
    }

    int t;
    if ((t=recv(clientSocket, buffer, BUFFER_LENGTH, 0)) > 0)
    {
        buffer[t] = '\0';
        char first = buffer[0];
        if(first == 'D')
        {
            exit(0);
        }
    }
    else
    {
        if (t < 0) perror("recv");
        else printf("Server closed connection\n");
        exit(1);
    }


    close(clientSocket);
}

/*************************************************/
/*                   OPTION -S                   */
/*************************************************/
void basicServer()
{

}

/*************************************************/
/*                   ULTIMATE                    */
/*                    CLIENT                     */
/*************************************************/
void ultimateClient(int argc, char *argv[])
{
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
    if(option[0] != '-')
    {
        ultimateClient(argc,argv);
    }
    else if(strcmp("-x", option) == 0)
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
    unlink(SOCKET_PATH);
    return 0;
}
