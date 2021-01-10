#pragma once

#include "point.h"


// Square block that makes up 1 terrain unit in a level
class Block {
public:
    float BLOCK_SIZE = 0.5;     // Box size multiplier, 1 yields 2x2, 2 is 4x4, 3 is 6x6

    float x = 0;        // x spatial coordinate
    float y = 0;        // y spatial coordinate
    float z = 0;        // z spatial coordinate

    box collisionBox;   // Collision "hitbox"


    // Initialize the block (create its collisionBox)
    void init(float set_x, float set_y, float set_z) {
        x = set_x;
        y = set_y;
        z = set_y;

        collisionBox.p_highest = { x + BLOCK_SIZE, y + BLOCK_SIZE, z + BLOCK_SIZE};
        collisionBox.p_lowest = {x - BLOCK_SIZE, y - BLOCK_SIZE, z - BLOCK_SIZE};
    };
};
