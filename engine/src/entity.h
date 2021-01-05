#pragma once

// Object that exists in a level and is updated once per tick
class Entity {
public:
    int id = 0;     // unique runtime id
    float x = 0;    // x spatial coordinate
    float y = 0;    // y spatial coordinate
    float z = 0;    // z spatial coordinate

    // Run every tick
    virtual void update();

    // Run every frame render
    virtual void render();

    // Destructor
    virtual ~Entity();
};