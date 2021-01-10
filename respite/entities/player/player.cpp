#include "player.h"
#include "engine/src/managers.h"

#include <iostream>

Player::Player() {
    collisionBox = {{1,1,1}, {-1,-1,-1}};
}

void Player::update() {
    // Print current x y coordinates
    printf("Pos x: %f, y:%f\n", pos.x, pos.y);

    float next_x = pos.x;
    float next_y = pos.y;
    float next_z = pos.z;

    // Process movement input
    if (window.getKeyState((int)'W') == KEY_PRESSED) { next_x -= move_speed; }
    if (window.getKeyState((int)'A') == KEY_PRESSED) { next_y -= move_speed; }
    if (window.getKeyState((int)'S') == KEY_PRESSED) { next_x += move_speed; }
    if (window.getKeyState((int)'D') == KEY_PRESSED) { next_y += move_speed; }

    // Check if place to move to is not occupied
    if (world.checkEntityCollision(this, next_x, next_y, next_z) == nullptr) {
        pos.x = next_x;
        pos.y = next_y;
        pos.z = next_z;
    }
}