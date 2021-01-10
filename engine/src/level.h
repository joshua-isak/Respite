#include <list>
#include "entity.h"

#pragma once


class Level {
public:

    list <Entity*> entities;    // List of all entities in a level

    // Run once when level is first created
    virtual void init();

    // Add an entity to the end of the entities list
    void addEntity(Entity *e);
};