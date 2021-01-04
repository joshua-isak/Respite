#include "level.h"

void Level::addEntity(Entity *e) {
    entities.push_front(e);
}