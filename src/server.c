#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

#include "../include/formula.h"


int Port = 9999;
char* Strategy;

int Parse_Arguments(int argc, char** argv)
{
    char* prog;

    prog = *argv;
    while (++argv, --argc > 0)
        if (**argv == '-')
            switch (*++ * argv) {
            case 'p':
                --argc;
                Port = atoi(*++argv);
                printf("Server port: %d\n", Port);

                break;
            case 'h':
                printf("\nHELP: try matinv -u \n\n");
                exit(0);
                break;
            case 'd':
                printf("\nDaemon Way-------------\n");
                break;
            case 's':
                --argc;
                Strategy = (*++argv);
                break;

            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
                printf("HELP: try %s -u \n\n", prog);
                break;
            }
}

int words(const char *sentence)
{
    int count=0,i,len;
    char lastC;
    len=strlen(sentence);
    if(len > 0)
    {
        lastC = sentence[0];
    }
    for(i=0; i<=len; i++)
    {
        if((sentence[i]==' ' || sentence[i]=='\0') && lastC != ' ')
        {
            count++;
        }
        lastC = sentence[i];
    }
    return count;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = (char**)malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int main(int argc, char** argv){

    Parse_Arguments(argc, argv);

    // Server socket id
    int sockfd, ret;
 
    // Server socket address structures
    struct sockaddr_in serverAddr;
 
    // Client socket id
    int clientSocket;
 
    // Client socket address structures
    struct sockaddr_in cliAddr;
 
    // Stores byte size of server socket address
    socklen_t addr_size;
 
    // Child process id
    pid_t childpid;
 
    // Creates a TCP socket id from IPV4 family
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
    // Error handling if socket id is not valid
    if (sockfd < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
 
    // printf("Server Socket is created.\n");
 
    // Initializing address structure with NULL
    memset(&serverAddr, '\0',
           sizeof(serverAddr));
 
    // Assign port number and IP address
    // to the socket created
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(Port);
 
    // 127.0.0.1 is a loopback address
    serverAddr.sin_addr.s_addr
        = inet_addr("127.0.0.1");
 
    // Binding the socket id with
    // the socket structure
    ret = bind(sockfd,
               (struct sockaddr*)&serverAddr,
               sizeof(serverAddr));
 
    // Error handling
    if (ret < 0) {
        printf("Error in binding.\n");
        exit(1);
    }
 
    // Listening for connections (upto 10)
    if (listen(sockfd, 10) == 0) {
        printf("Listening for clients...\n\n");
    }
 
    char buffer[2048];
    int client_id = 0;
    while (1) {
 
        // Accept clients and
        // store their information in cliAddr
        clientSocket = accept(
            sockfd, (struct sockaddr*)&cliAddr,
            &addr_size);
 
        // Error handling
        if (clientSocket < 0) {
            exit(1);
        }
 
        // Displaying information of
        // connected client
        // printf("Connection accepted from %s:%d\n",
        //        inet_ntoa(cliAddr.sin_addr),
        //        ntohs(cliAddr.sin_port));
 
        // Print number of clients
        // connected till now
        printf("Connected with client %d\n", ++client_id);
 
        // Creates a child process
        if ((childpid = fork()) == 0) {
 
            // Closing the server socket id
            close(sockfd);
            int matinvc = 0, kmeansc = 0;
            int argc = 0;
            char** argv;
            while (1)
            {
                buffer[0] = 0;
                if (recv(clientSocket, buffer, 2048, 0) < 0) {
                    printf("Error in receiving data.\n");
                }
                printf("Client %d command: %s\n", client_id, buffer);

                argc = words(buffer);
                argv = str_split(buffer, ' ');

                char fileName[128];

                if(strcmp(*argv, "matinvpar") == 0){
                    matinvc++;
                    sprintf(fileName, "matinv_client%d_soln%d.txt", client_id, matinvc);
                    main_matinv(argc, argv, matinvc, client_id);
                }
                else if(strcmp(*argv, "kmeanspar") == 0){
                    kmeansc++;
                    sprintf(fileName, "kmeans_client%d_soln%d.txt", client_id, kmeansc);
                    main_kmeans(argc, argv, kmeansc, client_id);
                }



                // Send a confirmation message
                // to the client
                
                printf("Sending solution: %s\n", fileName);
                send(clientSocket, fileName, strlen(fileName), 0);

            }
            
 
            
        }
    }
 
    // Close the client socket id
    close(clientSocket);

    return 0;
}

