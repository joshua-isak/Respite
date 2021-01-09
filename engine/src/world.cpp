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


Entity* World::checkEntityCollision(Entity *self, float x_pos, float y_pos, float z_pos) {
    point a_max = { self->collisionBox.p_highest.x + x_pos,
                    self->collisionBox.p_highest.y + y_pos,
                    self->collisionBox.p_highest.z + z_pos  };

    point a_min = { self->collisionBox.p_lowest.x + x_pos,
                    self->collisionBox.p_lowest.y + y_pos,
                    self->collisionBox.p_lowest.z + z_pos   };

    // Iterate through all entities in the world and check if self is colliding with any of them
    map<int, Entity*>::iterator it;

    for (it = entities.begin(); it != entities.end(); it++) {

        Entity *other = it->second;

        point b_max = { other->collisionBox.p_highest.x + x_pos,
                        other->collisionBox.p_highest.y + y_pos,
                        other->collisionBox.p_highest.z + z_pos  };

        point b_min = { other->collisionBox.p_lowest.x + x_pos,
                        other->collisionBox.p_lowest.y + y_pos,
                        other->collisionBox.p_lowest.z + z_pos   };

        // Actually check if the collision boxes intersect anywhere
        bool b1 = self->collisionBox.p_highest.x > other->collisionBox.p_lowest.x;
        bool b2 = self->collisionBox.p_lowest.x  < other->collisionBox.p_highest.x;

        bool b3 = self->collisionBox.p_highest.y > other->collisionBox.p_lowest.y;
        bool b4 = self->collisionBox.p_lowest.y  < other->collisionBox.p_highest.y;

        bool b5 = self->collisionBox.p_highest.z > other->collisionBox.p_lowest.z;
        bool b6 = self->collisionBox.p_lowest.z  < other->collisionBox.p_highest.z;

        if ( !(b1 && b2 && b3 && b4 && b5 && b6) ){
            return other;
        }
    }

    return nullptr;

}


