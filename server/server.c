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

void Server_listenClients(Server* server, int socketID, sockaddr_in* clientInfos, int id){
    Player* player = malloc(sizeof(Player));
    if(player == NULL){
        return;
    }

    player->playerID = id;
    player->socketID = socketID; 
    player->socketInfos = clientInfos;
    
    void** threadParams = (void**) calloc(2, sizeof(void*));
    threadParams[0] = player;
    threadParams[1] = server;

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
    
    
    printf("[Quiz in][server] New client sign in\n");

    char name[51];
    int longueur;

    if ((longueur = read(player->socketID, name, sizeof(name))) <= 0) 
        return NULL;

    name[longueur] = '\0';

    strcpy(player->pseudo, name);
    printf("[Quiz in][server] New client is %s\n", name);

    Server_sendQuestion(server, player, server->questions[0]);

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

void Server_printQuestion(Question *q) {
    printf("Question : %s\n", q->text);
    printf("  Réponse 1 : %s\n", q->answer[0]);
    printf("  Réponse 2 : %s\n", q->answer[1]);
    printf("  Réponse 3 : %s\n", q->answer[2]);
    printf("  Réponse 4 : %s\n", q->answer[3]);
}

Question* Server_getQuestionFromLine(char* line) {
    
    Question *q = (Question*) malloc(sizeof(Question));
   
   strncpy(q->text, strtok(line, ";"), 254);
   strncpy(q->answer[0], strtok(NULL, ";"), 254);
   strncpy(q->answer[1], strtok(NULL, ";"), 254);
   strncpy(q->answer[2], strtok(NULL, ";"), 254);
   strncpy(q->answer[3], strtok(NULL, ";"), 254);
   
    return q;
}

void Server_sendQuestion(Server* server, Player* player, Question *question){
    
    void* params = (void*)Server_shuffleAnswers(question);
    
    if(write(player->socketID, params, sizeof(params)) <= 0){
        printf("error\n");
        exit(0);
    }
     
    printf("[Quiz in][server] Question send to player #%d : %s\n", player->playerID, player->pseudo);
}

void swapping (char** elt1, char** elt2)
{
    char *temp = *elt1;
    *elt1 = *elt2;
    *elt2 = temp;
}

Question* Server_shuffleAnswers (Question* q)
{
    int n=4;
    char** arr=(char*[4]){"","","",""};
    for(int k=0;k<n;++k){
      //strcpy(arr[k],q->answer[k]);
      arr[k]=q->answer[k];
      //printf("arr[%d]=%s | ",k,arr[k]);
    }
    /*
    printf("tab avant : ");
    for (int it = 0; it < n; it++)
        printf("arr[%d]=%s ", it,arr[it]);
    printf("\n\n");
    */
    for (int i = n-1; i > 0; i--)
    {
        int j = rand() % (i+1);
        swapping(&arr[i], &arr[j]);
    }
    /*
    printf("tab apres : ");
    for (int ij = 0; ij < n; ij++)
        printf("arr[%d]=%s ", ij,arr[ij]);
    printf("\n\n");
    */
    for(int l=0;l<n;l++){
      //printf("BEFORE COPY : answer[%d]=%s AND arr[%d]=%s \n",l,q->answer[l],l,arr[l]);
      //strcpy(q->answer[l],arr[l]);
      //memset(q->answer[l],0,511);
      //memcpy(q->answer[l],arr[l],511);
      strncpy(q->answer[l], arr[l], 511);
      //q->answer[l]=arr[l];
      //printf("AFTER COPY : answer[%d]=%s AND arr[%d]=%s \n",l,q->answer[l],l,arr[l]);
    }
    //printf("\n");
    //Server_printQuestion(q);
    return q;
}