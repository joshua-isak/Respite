#include "thing.h"
#include "engine/src/managers/managers.h"

#include <iostream>

Thing::Thing() {
    collision_box = {{1,1,1}, {-1,-1,-1}};
    solid = true;
}

void Thing::update() {
    // Check if anything is colliding with self
    if (world.checkEntityCollision(this, x, y, z) != nullptr) {
        printf("Something is colliding with me!!!");
    }
}