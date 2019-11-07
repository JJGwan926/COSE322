/**
 *  author: Minsu Kim
 *  date: 2019.11.07
 *  description: client-side socket communication program
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>    // for thread

#define N_OF_PORT   5   // # of port = 5

void configureSocket(int clientSocket, int portList) {

    printf("configuring socket for port %d...\n", portList);

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);  // socket uses TCP
    if (clientSocket == -1) {
        printf("port %d socket error\n", portList);
    }

    printf("port %d configuration done!\n", portList);
}

int main(int argc, char* argv[]) {
    int clientSocket[N_OF_PORT], serverSocket[N_OF_PORT], portList[N_OF_PORT];
    struct sockaddr_in clientAddress[N_OF_PORT], serverAddress;

    int i;

    // using port number 1111, 2222, 3333, 4444, 5555
    for (i=0; i<N_OF_PORT; ++i) {
        portList[i] = (i+1) * 1111;
    }

    for (i=0; i<N_OF_PORT; ++i) {
        printf("Processing port %d...\n", portList[i]);
    
        // 1. configure sockets
        configureSocket(clientSocket[i], portList[i]);

        // 2. assigning a name to a sockets
        printf("assigning a name to the socket for port %d...\n", portList[i]);
        memset(&clientAddress[i], 0x00, sizeof(clientAddress[i]));   // init address to zero

        clientAddress[i].sin_family = AF_INET;  // IPv4
        clientAddress[i].sin_addr.s_addr = htonl(INADDR_ANY);   // TCP
        clientAddress[i].sin_port = htons(portList[i]); // port num

        if (bind(clientSocket[i], (struct sockaddr*)&clientAddress[i], sizeof(clientAddress[i])) < 0) {
            printf("port %d binding error\n", portList[i]);
        }
        printf("assigning a name to the socket for port %d done!\n", portList[i]);

        // 3. connect each socket to the server
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET; // IPv4
        serverAddress.sin_port = htons(portList[i]);    // port number
        serverAddress.sin_addr.s_addr = inet_addr("192.168.56.101");    // server addr

        if (connect(clientSocket[i], (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            printf("port %d connecting fail\n", portList[i]);
        }

        printf("Port %d process done!\n\n", portList[i]);
    }

    return 0;
}