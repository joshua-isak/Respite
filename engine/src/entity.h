#pragma once
#include <engine/lib/glm-master/glm/glm.hpp>

// A rectuangular prism defined by 2 points in 3D space
struct box {
    glm::vec3 p_highest;
    glm::vec3 p_lowest;
};


// Object that exists in a level and is updated once per tick
class Entity {
public:
    int id = 0;         // unique runtime id
    glm::vec3 pos;

    bool persistent;    // True to survive level changes

    box collisionBox;   // Collision "hitbox". Defined by coordinates relative to entity's x,y,z

    // Run every tick
    virtual void update();

    // Run every frame render
    virtual void render();

    // Destructor
    virtual ~Entity();
};