#pragma once

#include <vector>
#include <map>

#include "engine/src/level.h"
#include "engine/src/entity.h"
#include "engine/src/block.h"


class World {
public:

    std::map <int, Entity*> entities;        // map of all entities in the world

    std::map <point, Block*> terrain;        // map of all terrain blocks in the world

    // Initialize the world
    void init();

    // Destroy the world
    void destroy();

    // Call the update function on all entities in the world
    void update();

    // Add an entity to the world
    void addEntity (int id, Entity *e);

    // Create a block and add it to terrain
    void createBlock(float x_pos, float y_pos, float z_pos);

    // Loops through all (solid) entities in the world and returns the entity that entity self is colliding with at position x y z
    Entity* checkEntityCollision(Entity *self, float x, float y, float z);

    // Loop through all nearby (solid) blocks in the world surrounding self within collision_distance, and return the block that self is colliding with at position x y z
    Block* checkBlockCollision(Entity *self, float x_pos, float y_pos, float z_pos);

    // Check if there is a collision with any entity or nearby terrain
    bool checkIfCollision(Entity *self, float x_pos, float y_pos, float z_pos);

    // Destroy an entity of a given id
    void destroyEntity(int id);
};