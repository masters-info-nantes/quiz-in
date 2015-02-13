#include "server.h"

Server* Server_create(){
    Server* server = (Server*) malloc(sizeof(Server));
    
    if(server){
        server->socketID = -1;
        server->maxPool = MAX_POOL;

        server->players = calloc(MAX_POOL*2, sizeof(Player*));
        if(server->players){
            server->clientsThread = calloc(MAX_POOL*2, sizeof(pthread_t));

            if(server->clientsThread == NULL){
                free(server->players);
                free(server);
            }
        }
        else {
            free(server);
        }
    }

    srand(time(NULL)); // For real random
    
    return server;
}

bool Server_run(Server* server, int port){

    // Get local host name 
    char* serverName[HOSTNAME_MAX_LENGTH + 1] = { 0 };
    gethostname((char*)serverName, HOSTNAME_MAX_LENGTH);       
    
    // Get server informations from name
    hostent* serverInfos = gethostbyname((char*)serverName);
    if (serverInfos == NULL) {
        perror("[Quiz in][server] Cannot find server from given hostname\n");
        return false;
    }        

    // Fill socket infos with server infos
    sockaddr_in socketInfos;
    bcopy((char*)serverInfos->h_addr_list[0], (char*)&socketInfos.sin_addr, serverInfos->h_length);
    socketInfos.sin_family       = serverInfos->h_addrtype;     /* ou AF_INET */
    socketInfos.sin_addr.s_addr  = INADDR_ANY;           /* ou AF_INET */

    // Socket configuration and creation
    socketInfos.sin_port = htons(port);
    printf("[Quiz in][server] Listening on %d\n", ntohs(socketInfos.sin_port));

    server->socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socketID < 0) {
        perror("[Quiz in][server] Unable to create socket connexion\n");
        return false;
    }

    // Bind socket identifier with its informations
    if ((bind(server->socketID, (sockaddr*)(&socketInfos), sizeof(socketInfos))) < 0) {
        perror("[Quiz in][server] Unable to bind connexion address with the socket\n");
        return false;
    }

    // Listen incoming connexions
    listen(server->socketID, QUEUE_MAX_LENGTH); 

    return true;
}

void Server_listenClients(Server* server, int socketID, sockaddr_in* clientInfos){
    Player* player = malloc(sizeof(Player));
    if(player == NULL){
        return;
    }

    player->socketID = socketID; 
    player->socketInfos = clientInfos;

    void* threadParams[2] = {player, server};

    // Create thread dedicated to the new client
    int threadCreated = pthread_create(&server->clientsThread[0], 
                                       NULL, Server_clientThread,
                                       (void*)threadParams
    );

    if(threadCreated){
        perror("[Quiz in][server] Cannot create thread for new client\n");
        exit(1);
    }

    server->players[0] = player;
  
}

void* Server_clientThread(void* params) {

    void** paramList = (void**) params;
    Player* player = (Player*) paramList[0];
    Server* server = (Server*) paramList[1];

    printf("[Quiz in][server] New client connected #\n"); 
    //Player_printClientInfos(player);
    return NULL;
}

void Server_addAllQuestions(Server* server) {

    FILE *file;
    file = fopen("quiz-in.csv","r");
    int ch, number_of_lines = 0;

    do 
    {
        ch = fgetc(file);
        if(ch == '\n')
            number_of_lines++;
    } while (ch != EOF);

    // last line doesn't end with a new line!
    // but there has to be a line at least before the last line
    if(ch != '\n' && number_of_lines != 0) 
        number_of_lines++;
    fclose(file);   

    server->questions = calloc(number_of_lines, sizeof(Question*));
    if(server->questions){
        file = fopen("quiz-in.csv", "r");
        char line[2048];
        int pos = 0;
        while (fgets(line, 2048, file))
        {
            char* tmp = strdup(line);
            server->questions[pos] = Server_getQuestionFromLine(tmp);
            free(tmp);
            pos++;
        }
    }

    printf("[Quiz in][server] %d new questions are add on the server\n", number_of_lines); 
}

Question* Server_getQuestionFromLine(char* line)
{
    Question *q = (Question*) malloc(sizeof(Question));
   
   strncpy(q->text, strtok(line, ";"), 254);
   strncpy(q->answer[0], strtok(NULL, ";"), 254);
   strncpy(q->answer[1], strtok(NULL, ";"), 254);
   strncpy(q->answer[2], strtok(NULL, ";"), 254);
   strncpy(q->answer[3], strtok(NULL, ";"), 254);
   
    return q;
}