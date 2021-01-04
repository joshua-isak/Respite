#include "level.h"

void Level::init(){}

void Level::addEntity(Entity *e) {
    entities.push_front(e);
}

