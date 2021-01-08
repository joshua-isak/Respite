#pragma once

// A point in 3D space
struct point {
    float x = 0;
    float y = 0;
    float z = 0;
};

// A rectuangular prism defined by 3 points in 3D space
struct box {
    point p1;
    point p2;
    point p3;
};


// Object that exists in a level and is updated once per tick
class Entity {
public:
    int id = 0;         // unique runtime id
    float x = 0;        // x spatial coordinate
    float y = 0;        // y spatial coordinate
    float z = 0;        // z spatial coordinate

    box collisionBox;   // Collision "hitbox"

    // Run every tick
    virtual void update();

    // Run every frame render
    virtual void render();

    // Destructor
    virtual ~Entity();
};