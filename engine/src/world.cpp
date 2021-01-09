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

        // Don't check for a collision with ourselves
        if (other == self) { continue; }

        point b_max = { other->collisionBox.p_highest.x + other->x,
                        other->collisionBox.p_highest.y + other->y,
                        other->collisionBox.p_highest.z + other->z  };

        point b_min = { other->collisionBox.p_lowest.x + other->x,
                        other->collisionBox.p_lowest.y + other->y,
                        other->collisionBox.p_lowest.z + other->z   };

        // Actually check if the collision boxes intersect anywhere
        bool b1 = a_max.x > b_min.x;
        bool b2 = a_min.x < b_max.x;

        bool b3 = a_max.y > b_min.y;
        bool b4 = a_min.y < b_max.y;

        bool b5 = a_max.z > b_min.z;
        bool b6 = a_min.z < b_max.z;

        if (b1 && b2 && b3 && b4 && b5 && b6) {
            return other;
        }
    }

    return nullptr;

}


