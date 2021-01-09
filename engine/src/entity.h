#pragma once

// A point in 3D space
struct point {
    float x = 0;
    float y = 0;
    float z = 0;
};

// A rectuangular prism defined by 2 points in 3D space
struct box {
    point p_highest;
    point p_lowest;
};


// Object that exists in a level and is updated once per tick
class Entity {
public:
    int id = 0;         // unique runtime id
    float x = 0;        // x spatial coordinate
    float y = 0;        // y spatial coordinate
    float z = 0;        // z spatial coordinate

    bool persistent;    // True to survive level changes

    box collisionBox;   // Collision "hitbox". Defined by coordinates relative to entity's x,y,z

    // Run every tick
    virtual void update();

    // Run every frame render
    virtual void render();

    // Destructor
    virtual ~Entity();
};