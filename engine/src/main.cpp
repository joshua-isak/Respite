#include <iostream>
#include <list>
#include <iterator>

// Platform specific
#include <Windows.h>

// Engine stuff
#include "entity.h"
#include "level.h"

// Custom stuff, should not be here
#include "respite/entities/player/player.h"


using namespace std;


// class Entity {
// public:
//     int id;
//     float x;    // x coordinate
//     float y;
//     float z;

//     // Run every tick
//     virtual void update(){};

//     // Run every frame render
//     virtual void render(){};

// };


// class Player: public Entity {
// public:
//     int count = 0;

//     // Run every tick
//     void update() {
//         count += 1;
//         printf("Count: %d", count);
//     }
// };



// class Level {
// public:

//     list <Entity*> entities;    // List of all entities in a level

//     // Add an entity to the end of the entities list
//     void addEntity(Entity *e) {
//         entities.push_front(e);
//     }
// };


// Update all entities in a room
void update(Level l) {
    // Iterate through each entity in the level and run their update function

    list<Entity*>::iterator it;

    for (it = l.entities.begin(); it != l.entities.end(); it++) {
        (*it)->update();
    }
}


int main(int argc, char *argv[]) {

    // Get command line arguments-
    int tickrate = 5; //(int) argv[1];

    // Load the main level
    Level level;

    // Add a player entity to the level
    Player p;
    level.addEntity(&p);


    printf("starting game loop\n");

    // Main game loop
    while (true) {

        //processInput();
        update(level);
        //render();

        Sleep(1000/tickrate);  //expand this to account for simulation lag and render lag
    }
}