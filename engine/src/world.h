#pragma once

#include <vector>
#include <map>

#include "engine/src/level.h"
#include "engine/src/entity.h"
//#include "engine/src/block.h"


class World {
public:

    map <int, Entity*> entities;    // map of all entities in the world

    //vector <Block*> terrain;        // vector of all terrain blocks in the world

    // Initialize the world
    void init();

    // Destroy the world
    void destroy();

    // Call the update function on all entities in the world
    void update();

    // Add an entity to the world
    void addEntity (int id, Entity *e);

    // Destroy an entity of a given id
    void destroyEntity(int id);
};