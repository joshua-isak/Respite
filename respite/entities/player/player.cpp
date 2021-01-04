#include "player.h"

#include <iostream>

void Player::update(){
    count += 1;
    printf("Count: %d", count);
}