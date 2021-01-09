#include "player.h"
#include "engine/src/managers.h"

#include <iostream>

void Player::update(){
    // Print current x y coordinates
    printf("Pos x: %f, y:%f\n", x, y);
    
    // Process keyboard input
    if (window.getKeyState((int)'W') == KEY_PRESSED) { x -= 0.5; }
    if (window.getKeyState((int)'A') == KEY_PRESSED) { y -= 0.5; }
    if (window.getKeyState((int)'S') == KEY_PRESSED) { x += 0.5; }
    if (window.getKeyState((int)'D') == KEY_PRESSED) { y += 0.5; }
}