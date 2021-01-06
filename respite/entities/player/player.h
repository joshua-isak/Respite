#include "engine/src/entity.h"

#pragma once

// Entity that is used to control the player
class Player: public Entity {
public:

    // Run every tick
    void update();
};