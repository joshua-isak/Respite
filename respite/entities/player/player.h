#include "engine/src/entity.h"

#pragma once

// Entity that is used to control the player
class Player: public Entity {
public:
    int count = 0;      // stores how many times the update function ran

    // Run every tick
    void update();
};