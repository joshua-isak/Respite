#include "thing.h"
#include "engine/src/managers.h"

#include <iostream>

Thing::Thing() {
    collision_box = {{1,1,1}, {-1,-1,-1}};
    solid = true;
}

void Thing::update() {
    // Check if anything is colliding with self
    if (world.checkEntityCollision(this, pos.x, pos.y, pos.z) != nullptr) {
        printf("Something is colliding with me!!!");
    }
}