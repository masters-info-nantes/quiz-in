#include "client.h"

Client* Client_create(){

    Client* client = (Client*)malloc(sizeof(Client));

    if(client){
        client->socketID = -1;
        client->socketInfos = (sockaddr_in*) malloc(sizeof(sockaddr_in));

        if(client->socketInfos == NULL){
            free(client);
        }
    }

    return client;
}

bool Client_run(Client* client, char* serverName, int serverPort){

    hostent* serverInfos = gethostbyname(serverName);
    if (serverInfos == NULL) {
       perror("[CQuiz in] Cannot find server from given hostname\n");
       return false;
    }

    // Fill socket infos with server infos
    bcopy((char*)serverInfos->h_addr, (char*)&client->socketInfos->sin_addr, serverInfos->h_length);
    client->socketInfos->sin_family = AF_INET;
    client->socketInfos->sin_port = htons(serverPort);

    // Socket configuration and creation
    printf("[Quiz in] Connect to server %s:%d\n", serverInfos->h_name, ntohs(client->socketInfos->sin_port));
        
    client->socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socketID < 0) {
        perror("[Quiz in] Unable to create socket connexion\n");
        return false;
    }
    
    if ((connect(client->socketID, (sockaddr*)(client->socketInfos), sizeof(sockaddr))) < 0) {
        perror("[Quiz in] Cannot connect to the server\n");
        return false;
    }
    printf("[Quiz in] Connection sucessfull\n");
    
    return true;
}

void Client_setPseudo(Client* client){
    char pseudo[50];
    printf("[Quiz in] Choose your pseudo\n");
    scanf("%s", pseudo);
    printf("[Quiz in] Hello %s\n", pseudo);

    if(write(client->socketID, pseudo, sizeof(pseudo)) <= 0){
        printf("error\n");
        exit(0);
    }
}

void Client_printQuestion(Question q) {
    printf("Question : %s\n", q.text);
    printf("  Réponse 1 : %s\n", q.answer[0]);
    printf("  Réponse 2 : %s\n", q.answer[1]);
    printf("  Réponse 3 : %s\n", q.answer[2]);
    printf("  Réponse 4 : %s\n", q.answer[3]);
}

void Client_getQuestion(Client* client){
    Question q;
    int clientStatus = read(client->socketID, &q, sizeof(q));
    Client_printQuestion(q);
}