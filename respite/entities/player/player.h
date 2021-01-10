#include "engine/src/entity.h"

#pragma once

// Entity that is used to control the player
class Player: public Entity {
public:
    float move_speed = 0.1;     // x y movement speed

    // Constructor
    Player();

    // Run every tick
    void update();
};