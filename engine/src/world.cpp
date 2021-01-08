#include "world.h"

#include "respite/respite.h"

void World::init() {}


void World::update(){

    map<int, Entity*>::iterator it;

    for (it = entities.begin(); it != entities.end(); it++) {
        it->second->update();
    }

}


void World::addEntity (int id, Entity *e) {
    entities.insert({id, e});
}




