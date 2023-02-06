#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int serverPort = 9999;
char* serverIp = "127.0.0.1";

void Parse_Argument(int argc, char** argv){
    char* prog;

    prog = *argv;
    while (++argv, --argc > 0)
        if (**argv == '-')
            switch (*++ * argv) {
            case 'i':
                if((*++ * argv) == 'p'){
                    --argc;
                    serverIp = (*++argv);
                    printf("Server ip: %s\n", serverIp);
                }                
                break;
            case 'p':
                --argc;
                serverPort = atoi(*++argv);
                printf("Server port: %d\n\n", serverPort);
                break;
            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
                printf("HELP: try %s -u \n\n", prog);
                break;
            }

}

int main(int argc, char** argv){
    Parse_Argument(argc, argv);

    // Socket id
    int clientSocket, ret;
 
    // Client socket structure
    struct sockaddr_in cliAddr;
 
    // char array to store incoming message
    char buffer[2048];
 
    // Creating socket id
    clientSocket = socket(AF_INET,
                          SOCK_STREAM, 0);
 
    if (clientSocket < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
    // printf("Client Socket is created.\n");
 
    // Initializing socket structure with NULL
    memset(&cliAddr, '\0', sizeof(cliAddr));
 
    // Initializing buffer array with NULL
    memset(buffer, '\0', sizeof(buffer));
 
    // Assigning port number and IP address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
 
    // 127.0.0.1 is Loopback IP
    serverAddr.sin_addr.s_addr
        = inet_addr(serverIp);
 
    // connect() to connect to the server
    ret = connect(clientSocket,
                  (struct sockaddr*)&serverAddr,
                  sizeof(serverAddr));
 
    if (ret < 0) {
        printf("Error in connection.\n");
        exit(1);
    }
 
    printf("Connected to Server.\n");
 
    char* line = NULL;
    size_t len = 0;
    size_t read = 0;
    while (1) {
        printf("Enter a command for the server: ");

        read = getline(&line, &len, stdin);
        // printf("%s\n", line);
        send(clientSocket, line, strlen(line), 0);
 
        // recv() receives the message
        // from server and stores in buffer
        if (recv(clientSocket, buffer, 2048, 0) < 0) {
            printf("Error in receiving data.\n");
        }

        printf("Received the solution: %s\n", buffer); 

    }
 
    return 0;
}