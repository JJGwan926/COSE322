/**
 *  author: Minsu Kim
 *  date: 2019.11.07
 *  description: client-side socket communication program
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>    // for thread

#define N_OF_PORT   5       // # of port = 5
#define MSG_SIZE    65536   // max size of received message

// set five port number with seed
void init5Ports(int* portNumbers, int seed) {

    for (int i=0; i<N_OF_PORT; ++i) {
        portNumbers[i] = (i+1) * seed;
    }
    
    return;
}

// set client socket
void configureSocket(int* clientSocket, int portNumber) {

    printf("configuring socket for port %d...\n", portNumber);

    *clientSocket = socket(PF_INET, SOCK_STREAM, 0);  // socket uses TCP
    if (*clientSocket == -1) {  // error
        printf("port %d socket error\n", portNumber);
    }

    printf("port %d configuration done!\n", portNumber);

    return;
}

// naming socket
void socketNaming(struct sockaddr_in* clientAddress, int clientSocket, int portNumber) {

    printf("assigning a name to the socket for port %d...\n", portNumber);
    memset(clientAddress, 0x00, sizeof(*clientAddress));   // clear address as zero

    clientAddress->sin_family = AF_INET;  // IPv4
    clientAddress->sin_addr.s_addr = htonl(INADDR_ANY);   // TCP
    clientAddress->sin_port = htons(portNumber); // port num

    if (bind(clientSocket, (struct sockaddr*)clientAddress, sizeof(*clientAddress)) < 0) {  // error check
        printf("port %d binding error\n", portNumber);
    }
    
    printf("assigning a name to the socket for port %d done!\n", portNumber);

    return;
}

// connect socket to server
void connect2Server(struct sockaddr_in* serverAddress, int clientSocket, int portNumber) {

    memset(serverAddress, 0, sizeof(*serverAddress));
    serverAddress->sin_family = AF_INET; // IPv4
    serverAddress->sin_port = htons(portNumber);    // port number
    serverAddress->sin_addr.s_addr = inet_addr("192.168.56.101");    // server addr

    if (connect(clientSocket, (struct sockaddr*)serverAddress, sizeof(*serverAddress)) < 0) {
        printf("port %d connecting fail\n", portNumber);
    }

    return;
}

char* time2String(struct timeval *t, struct tm *times) {
    
    static char s[20];
    
    sprintf(s, "%02d:%02d:%02d.%02ld\n", times->tm_hour, times->tm_min, times->tm_sec, t->tv_usec);

    return s;
}

char* getCurrentTime() {
struct timeb itb;

    struct tm   *lt;
    static char s[20];

    ftime(&itb);

    lt = localtime(&itb.time);

    sprintf(s, "%02d:%02d:%02d.%03d"
            , lt->tm_hour, lt->tm_min, lt->tm_sec
            , itb.millitm);

    return s;
}

void receiveServerMsg(int clientSocket) {

    int         msgLen;         // length of received message
    char        msg[MSG_SIZE];  // received message    

    while ( (msgLen = recv(clientSocket, msg, 10, 0) ) != -1) {
        printf("Received message: %s at %s\n", msg, getCurrentTime());
    }

    return;
}

void closeSockets(int* clientSocket) {
    
    for (int i=0; i<N_OF_PORT; ++i) {
        close(clientSocket[i]);
    }

    return;
}

int main(int argc, char* argv[]) {

    int clientSocket[N_OF_PORT];    // client socket
    int portNumbers[N_OF_PORT];     // port number for each sockets
    struct sockaddr_in clientAddress[N_OF_PORT];    // address for each client
    struct sockaddr_in serverAddress;               // server address

    pthread_t   pThread[N_OF_PORT]; // thread
    int         threadId;           // thread id
    char        message[MSG_SIZE];  // received message from server

    int i;  // index

    // using port number 1111, 2222, 3333, 4444, 5555
    init5Ports(portNumbers, 1111);

    // connect each sockets to the server
    for (i=0; i<N_OF_PORT; ++i) {
        printf("Processing port %d...\n", portNumbers[i]);
    
        // 1. configure a socket
        configureSocket(&clientSocket[i], portNumbers[i]);

        // 2. assigning a name to a socket
        socketNaming(&clientAddress[i], clientSocket[i], portNumbers[i]);

        // 3. connect each socket to the server
        connect2Server(&serverAddress, clientSocket[i], portNumbers[i]);

        printf("Port %d process done!\n\n", portNumbers[i]);
    }

    receiveServerMsg(clientSocket[0]);

    // close sockets
    closeSockets(clientSocket); 

    return 0;
}