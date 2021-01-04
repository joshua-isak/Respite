#include <list>
#include "entity.h"

#pragma once

using namespace std;

class Level {
public:

    list <Entity*> entities;    // List of all entities in a level

    // Add an entity to the end of the entities list
    void addEntity(Entity *e);
};