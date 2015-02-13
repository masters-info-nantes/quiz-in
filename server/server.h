#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "player.h"
#include "question.h"

#define MAX_POOL 10
#define HOSTNAME_MAX_LENGTH 256
#define QUEUE_MAX_LENGTH 5

typedef struct sockaddr sockaddr;
typedef struct hostent hostent;

typedef struct _Server Server;
struct _Server {
    int socketID;
    int maxPool;

    Player** players;
    pthread_t* clientsThread;
    Question** questions;
};

Server* Server_create();
bool Server_run(Server* server, int port);
void Server_listenClients(Server* server, int socketID, sockaddr_in* clientInfos);
void* Server_clientThread(void* params);
void Server_addAllQuestions(Server* server);
Question* Server_getQuestionFromLine(char* line);

#endif