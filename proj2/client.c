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

void configureSocket(int clientSocket, int portNumber) {

    printf("configuring socket for port %d...\n", portNumber);

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);  // socket uses TCP
    if (clientSocket == -1) {
        printf("port %d socket error\n", portNumber);
    }

    printf("port %d configuration done!\n", portNumber);

    return;
}

void socketNaming(struct sockaddr_in clientAddress, int clientSocket, int portNumber) {

    printf("assigning a name to the socket for port %d...\n", portNumber);
    memset(&clientAddress, 0x00, sizeof(clientAddress));   // init address to zero

    clientAddress.sin_family = AF_INET;  // IPv4
    clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);   // TCP
    clientAddress.sin_port = htons(portNumber); // port num

    if (bind(clientSocket, (struct sockaddr*)&clientAddress, sizeof(clientAddress)) < 0) {
        printf("port %d binding error\n", portNumber);
    }
    
    printf("assigning a name to the socket for port %d done!\n", portNumber);

    return;
}

void connect2Server(struct sockaddr_in serverAddress, int clientSocket, int portNumber) {
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET; // IPv4
    serverAddress.sin_port = htons(portNumber);    // port number
    serverAddress.sin_addr.s_addr = inet_addr("192.168.56.101");    // server addr

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("port %d connecting fail\n", portNumber);
    }

    printf("Port %d process done!\n\n", portNumber);
}

int main(int argc, char* argv[]) {

    int clientSocket[N_OF_PORT], serverSocket[N_OF_PORT], portNumbers[N_OF_PORT];
    struct sockaddr_in clientAddress[N_OF_PORT], serverAddress;

    int i;

    // using port number 1111, 2222, 3333, 4444, 5555
    for (i=0; i<N_OF_PORT; ++i) {
        portNumbers[i] = (i+1) * 1111;
    }

    for (i=0; i<N_OF_PORT; ++i) {
        printf("Processing port %d...\n", portNumbers[i]);
    
        // 1. configure sockets
        configureSocket(clientSocket[i], portNumbers[i]);

        // 2. assigning a name to a sockets
        socketNaming(clientAddress[i], clientSocket[i], portNumbers[i]);

        // 3. connect each socket to the server
        connect2Server(serverAddress, clientSocket[i], portNumbers[i]);

        printf("Port %d process done!\n\n", portList[i]);
    }

    return 0;
}