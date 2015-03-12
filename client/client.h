#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>

#include "question.h"

#define MAX_BUFFER_SIZE 256

typedef struct sockaddr 	sockaddr;
typedef struct sockaddr_in 	sockaddr_in;
typedef struct hostent 		hostent;

typedef struct _Client Client;
struct _Client {
	int socketID;
	sockaddr_in* socketInfos;
};

Client* Client_create();
bool Client_run(Client* client, char* serverName, int serverPort);

void Client_setPseudo(Client* client);
Question Client_getQuestion(Client* client);
void Client_printQuestion(Question q);
void Client_setResponse(Client* client, Question q);
void Client_getResponse(Client* client);
void Client_sendOK(Client* client);

#endif