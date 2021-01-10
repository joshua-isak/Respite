#pragma once

#include "point.h"

// Object that exists in a level and is updated once per tick
class Entity {
public:
    int id = 0;         // unique runtime id

    float x = 0;        // x spatial coordinate
    float y = 0;        // y spatial coordinate
    float z = 0;        // z spatial coordinate

    bool persistent = false;    // set true to survive level changes

    bool solid = false;         // set false to skip collision checking
    box collision_box;           // Collision "hitbox". Defined by coordinates relative to entity's x,y,z

    // Run every tick
    virtual void update();

    // Run every frame render
    virtual void render();

    // Destructor
    virtual ~Entity();
};