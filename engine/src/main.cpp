#include <iostream>
#include <list>
#include <iterator>
#include <thread>
#include <chrono>

// Platform specific
//#include <Windows.h>

// Engine stuff
#include "entity.h"
#include "level.h"
//#include "window.h"
//include "world.h"

// Respite game stuff, should ideally not be here
#include "respite/respite.h"

// EXTERN MANAGERS
#include "engine/src/managers/managers.h"

using namespace std;



// Update all entities in a room
void update(Level *lvl) {
    // Dereference the level to be updated
    Level l = *lvl;

    // Iterate through each entity in the level and run their update function
    list<Entity*>::iterator it;

    for (it = l.entities.begin(); it != l.entities.end(); it++) {
        (*it)->update();
    }
}

// Manager declarations
Window window;                  // window manager
World world;                    // world manager


int main(int argc, char *argv[]) {
    // Engine parameters
    int tickrate = 60;          // simulation cycles per second

    // Engine variables
    Level *current_level;       // current level to simulate and display
    bool running = true;        // current engine state (false breaks main game loop)

    // Initialize Managers
    window.init(640, 480, "Respite", &running);
    world.init();


    //////// CUSTOM GAME TEST CODE ////////
    Player *p = new Player;
    p->x = 0;
    p->y = 4;
    world.addEntity(1, p);

    Thing *t0 = new Thing;
    t0->x = 10;
    world.addEntity(2, t0);

    // Thing *t1 = new Thing;
    // t1->y = 2;
    // world.addEntity(3, t1);

    world.createBlock(0,0,0);
    world.createBlock(0,1,0);
    world.createBlock(0,2,0);
    world.createBlock(0,3,0);

    Block *b = world.terrain[point{0,0,0}];

    ///////////////////////////////////////


    printf("starting game loop\n");

    // Main game loop
    while (running) {

        window.processInput();
        world.update();
        //render();

        //expand this to account for simulation lag and render lag
        this_thread::sleep_for(chrono::milliseconds(1000/tickrate));
    }

    // Shutdown managers (reverse order from declarations)
    world.destroy();
    window.destroy();

}