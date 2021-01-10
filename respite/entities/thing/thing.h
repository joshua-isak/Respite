#include "engine/src/entity.h"

#pragma once

// Entity that is used to control the player
class Thing: public Entity {
public:

    // Constructor
    Thing();

    // Run every tick
    void update();
};