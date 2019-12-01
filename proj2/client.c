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
#include <pthread.h> // for thread

#define N_OF_PORT 5    // # of port = 5
#define MSG_SIZE 65536 // max size of received message

struct _pthreadArgs {
    int *clientSocket;
    int portNumber;
};
typedef struct _pthreadArgs PThreadArgs;

// set five port number with seed
void init5Ports(int *portNumbers) {

    portNumbers[0] = 4444;
    portNumbers[1] = 5555;
    portNumbers[2] = 6666;
    portNumbers[3] = 7778;
    portNumbers[4] = 33333; // *

    return;
}

// set client socket
void configureSocket(int *clientSocket, int portNumber) {

    printf("configuring socket for port %d...", portNumber);

    *clientSocket = socket(PF_INET, SOCK_STREAM, 0); // socket uses TCP
    if (*clientSocket == -1) { // error
        printf("\nport %d socket error\n", portNumber);
        exit(1);
    }

    printf("done.\n");

    return;
}

// naming socket
void socketNaming(struct sockaddr_in *clientAddress, int clientSocket, int portNumber) {

    printf("assigning a name to the socket for port %d...", portNumber);

    memset(clientAddress, 0x00, sizeof(*clientAddress)); // clear address as zero

    clientAddress->sin_family = AF_INET;                // IPv4
    clientAddress->sin_addr.s_addr = htonl(INADDR_ANY); // TCP
    clientAddress->sin_port = htons(portNumber);        // port num

    if (bind(clientSocket, (struct sockaddr *)clientAddress, sizeof(*clientAddress)) < 0) { // error check
        printf("\nport %d binding error\n", portNumber);
        exit(1);
    }

    printf("done.\n");

    return;
}

// configure server
void configureServer(struct sockaddr_in *serverAddress, char *serverIp, int portNumber) {

    memset(serverAddress, 0, sizeof(*serverAddress));

    serverAddress->sin_family = AF_INET;                  // IPv4
    serverAddress->sin_port = htons(portNumber);          // port number
    serverAddress->sin_addr.s_addr = inet_addr(serverIp); // server addr

    return;
}

// connect socket to server
void connect2Server(struct sockaddr_in *serverAddress, int clientSocket, int portNumber) {

    printf("port %d connecting to server...", portNumber);

    if (connect(clientSocket, (struct sockaddr *)serverAddress, sizeof(*serverAddress)) < 0) {
        printf("\nport %d connecting fail\n", portNumber);
        exit(1);
    }

    printf("done.\n");

    return;
}

char *getCurrentTime() {

    struct timeb itb;
    struct tm *lt;
    static char s[20];

    ftime(&itb);

    lt = localtime(&itb.time);

    sprintf(s, "%02d:%02d:%02d.%03d", lt->tm_hour, lt->tm_min, lt->tm_sec, itb.millitm);

    return s;
}

void *receiveServerMsg(void *pThreadArgs) {

    int msgLen;         // length of received message
    char msg[MSG_SIZE]; // received message
    int socketFd = *(((PThreadArgs *)pThreadArgs)->clientSocket);
    int portNumber = ((PThreadArgs *)pThreadArgs)->portNumber;

    FILE *fp;
    char logPath[20];

    sprintf(logPath, "./log/%d.txt", portNumber); // save path

    fp = fopen(logPath, "w"); // open file
    if (!fp) { // error
        printf("file open failed\n");
        exit(1);
    }

    //    while ( (msgLen = recv(socketFd, msg, 10, 0) ) != -1) {
    for (int i = 0; i < 1000; ++i) {
        char logMsg[80];
        msgLen = recv(socketFd, msg, MSG_SIZE, 0);
        sprintf(logMsg, "%s  %d  %s", getCurrentTime(), msgLen, msg);
        fputs(logMsg, fp); // write message to log
        fputs("\n", fp);
    }

    fclose(fp); // close file

    return pThreadArgs;
}

void createThread(pthread_t *pThread, int *clientSocket, int portNumber) {

    printf("creating thread... ");

    int threadId;                                                          // thread id
    PThreadArgs *pThreadArgs = (PThreadArgs *)malloc(sizeof(pThreadArgs)); // pthread function arguments
    pThreadArgs->clientSocket = clientSocket;
    pThreadArgs->portNumber = portNumber;

    threadId = pthread_create(pThread, NULL, receiveServerMsg, (void *)pThreadArgs); // create thread

    if (threadId < 0) { // error
        perror("thread creation error");
        exit(0);
    }

    printf("done.\n");

    return;
}

void closeSockets(int *clientSocket) {

    for (int i = 0; i < N_OF_PORT; ++i) {
        close(clientSocket[i]);
    }

    return;
}

int main(int argc, char *argv[]) {

    int clientSocket[N_OF_PORT];                 // client socket
    int portNumbers[N_OF_PORT];                  // port number for each sockets
    char serverIp[20];                           // server ip address
    struct sockaddr_in clientAddress[N_OF_PORT]; // address for each client
    struct sockaddr_in serverAddress;            // server address

    pthread_t pThread[N_OF_PORT]; // thread
    char message[MSG_SIZE];       // received message from server
    int status;

    int i; // index

    printf("Enter server IPv4 address: ");
    scanf("%s", serverIp);

    // using port number 4444, 5555, 6666, 7778, '33333'
    init5Ports(portNumbers);

    // connect each sockets to the server
    for (i = 0; i < N_OF_PORT; ++i) {
        printf("Processing port %d...\n", portNumbers[i]);

        // 1. configure a socket
        configureSocket(&clientSocket[i], portNumbers[i]);

        // 2. assigning a name to a socket
        socketNaming(&clientAddress[i], clientSocket[i], portNumbers[i]);

        // 3. configure server
        configureServer(&serverAddress, serverIp, portNumbers[i]);

        // 4. connect each socket to the server
        connect2Server(&serverAddress, clientSocket[i], portNumbers[i]);

        printf("Port %d process done!\n\n", portNumbers[i]);
    }

    // threads
    for (i = 0; i < N_OF_PORT; ++i) {
        // create thread
        createThread(&pThread[i], &clientSocket[i], portNumbers[i]);
    }
    for (i = 0; i < N_OF_PORT; ++i) {
        pthread_join(pThread[i], (void *)&status);
    }

    closeSockets(clientSocket);

    return 0;
}
