#include "world.h"

#include <cmath>

using namespace std;

void World::init() {}

void World::destroy() {}


void World::update(){

    map<int, Entity*>::iterator it;

    for (it = entities.begin(); it != entities.end(); it++) {
        it->second->update();
    }

}


void World::addEntity (int id, Entity *e) {
    entities.insert({id, e});
}


void World::createBlock(float x_pos, float y_pos, float z_pos) {
    Block *b = new Block;
    b->init(x_pos, y_pos, z_pos);
    terrain.insert({point{x_pos, y_pos, z_pos}, b});
}


Block* World::checkBlockCollision(Entity *self, float x_pos, float y_pos, float z_pos) {

    // Generate collision box relative to x_pos...z_pos for self
    point a_max = { self->collision_box.p_highest.x + x_pos,
                    self->collision_box.p_highest.y + y_pos,
                    self->collision_box.p_highest.z + z_pos  };

    point a_min = { self->collision_box.p_lowest.x + x_pos,
                    self->collision_box.p_lowest.y + y_pos,
                    self->collision_box.p_lowest.z + z_pos   };

    // Round position to the nearest whole number to find blocks to iterate through
    float r_x = round(x_pos);
    float r_y = round(y_pos);
    float r_z = round(z_pos);

    // Iterate through all blocks in a X by X by X area around self
    float col_dist = self->collision_distance;
    for (float i_x = (r_x - col_dist); i_x < (r_x + col_dist); i_x++) {
        for (float i_y = (r_y - col_dist); i_y < (r_y + col_dist); i_y++) {
            for (float i_z = (r_z - col_dist); i_z < (r_z + col_dist); i_z++) {

                Block *other = terrain[point{i_x, i_y, i_z}];

                // Skip blocks that don't exist
                if (other == nullptr) { continue; }

                // Skip blocks that are not solid
                // NYI

                // Check for a collision
                point b_max = { other->collision_box.p_highest.x + other->x,
                                other->collision_box.p_highest.y + other->y,
                                other->collision_box.p_highest.z + other->z  };

                point b_min = { other->collision_box.p_lowest.x + other->x,
                                other->collision_box.p_lowest.y + other->y,
                                other->collision_box.p_lowest.z + other->z   };

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
        }
    }

    return nullptr;

}


Entity* World::checkEntityCollision(Entity *self, float x_pos, float y_pos, float z_pos) {

    // Generate collision box relative to x_pos...z_pos for self
    point a_max = { self->collision_box.p_highest.x + x_pos,
                    self->collision_box.p_highest.y + y_pos,
                    self->collision_box.p_highest.z + z_pos  };

    point a_min = { self->collision_box.p_lowest.x + x_pos,
                    self->collision_box.p_lowest.y + y_pos,
                    self->collision_box.p_lowest.z + z_pos   };



    // Iterate through all entities in the world and check if self is colliding with any of them
    map<int, Entity*>::iterator it;

    for (it = entities.begin(); it != entities.end(); it++) {

        Entity *other = it->second;

        // Skip objects that are not solid
        if (other->solid == false) { continue; }

        // Don't check for a collision with ourselves
        if (other == self) { continue; }


        point b_max = { other->collision_box.p_highest.x + other->x,
                        other->collision_box.p_highest.y + other->y,
                        other->collision_box.p_highest.z + other->z  };

        point b_min = { other->collision_box.p_lowest.x + other->x,
                        other->collision_box.p_lowest.y + other->y,
                        other->collision_box.p_lowest.z + other->z   };

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


bool World::checkIfCollision(Entity *self, float x_pos, float y_pos, float z_pos) {
    // Use checkBlockCollision and checkEntityCollision to check if there is a collision
    if (checkBlockCollision(self, x_pos, y_pos, z_pos)) {
        return true;
    }
    if (checkEntityCollision(self, x_pos, y_pos, z_pos)){
        return true;
    }
    return false;
}