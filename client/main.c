#include "client.h"

int main(int argc, char **argv) {
  
    if (argc != 3) {
       perror("[Quiz in] Usage: client <serveur-ip> <server-port>\n");
       exit(1);
    }
   
    Client* client = Client_create();
    if(client == NULL){
        perror("[Quiz in] Unable to create client");
        exit(1);
    }

    char* serverName = argv[1]; 
    int serverPort = atoi(argv[2]);    
    if(!Client_run(client, serverName, serverPort))
        exit(1);
    
    Client_setPseudo(client);

}