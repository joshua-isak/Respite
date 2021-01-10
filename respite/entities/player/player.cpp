#include "player.h"
#include "engine/src/managers/managers.h"

#include <iostream>

Player::Player() {
    collision_box = {{1,1,1}, {-1,-1,-1}};
    solid = true;
}

void Player::update() {
    // Print current x y coordinates
    printf("Pos x: %f, y:%f\n", x, y);

    float next_x = x;
    float next_y = y;
    float next_z = z;

    // Process movement input
    if (window.getKeyState((int)'W') == KEY_PRESSED) { next_x -= move_speed; }
    if (window.getKeyState((int)'A') == KEY_PRESSED) { next_y -= move_speed; }
    if (window.getKeyState((int)'S') == KEY_PRESSED) { next_x += move_speed; }
    if (window.getKeyState((int)'D') == KEY_PRESSED) { next_y += move_speed; }

    // Check if place to move to is not occupied
    if (!world.checkIfCollision(this, next_x, y, z)) { x = next_x; }

    if (!world.checkIfCollision(this, x, next_y, z)) { y = next_y; }
}