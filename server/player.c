#include "player.h"

void Player_printClientInfos(Player* player){
    printf("* Remote IP: %s\n", inet_ntoa(player->socketInfos->sin_addr));    
    printf("* Remote port: %d\n\n", ntohs(player->socketInfos->sin_port));    
}