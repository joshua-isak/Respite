#pragma once

// Object that exists in a level and is updated once per tick
class Entity {
public:
    int id;     // unique runtime id
    float x;    // x spatial coordinate
    float y;    // y spatial coordinate
    float z;    // z spatial coordinate

    // Run every tick
    virtual void update();

    // Run every frame render
    virtual void render();

};