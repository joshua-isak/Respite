#include "player.h"

#include <iostream>

void Player::update(){
    count += 1;
    printf("Count: %d\n", count);
    printf("Pos x: %d, y:%d\n", x, y);
}