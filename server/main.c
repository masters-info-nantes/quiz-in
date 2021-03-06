#include "server.h"
#include "question.h"

int main(int argc, char **argv) {
    
    if (argc != 2) {
       perror("[Quiz in] Usage: server <port-to-listen>\n");
       exit(1);
    }

    Server* server = Server_create();
    if(server == NULL){
        perror("[Quiz in][server] Unable to create server");
        exit(1);
    }

    int port = atoi(argv[1]);
    if(!Server_run(server, port)){
        perror("[Quiz in][server] Unable to start server");
        exit(1);
    }

    Server_addAllQuestions(server);

    int id = 1;
    while(true){
        sockaddr_in* clientInfos = malloc(sizeof(sockaddr_in));
        socklen_t clientInfosSize = sizeof(clientInfos);
        int socketID = accept(server->socketID, (sockaddr*)clientInfos, &clientInfosSize);
        if (socketID < 0) {
            perror("[Quiz in][server] Cannot initiate connexion with a new client\n");
            exit(1);
        }
        Server_listenClients(server, socketID, clientInfos, id);
        id = id+1;  
    }


}