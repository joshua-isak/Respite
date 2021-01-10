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
#include "window.h"
#include "world.h"

// Respite game stuff, should ideally not be here
#include "respite/respite.h"

// EXTERN MANAGERS
#include "managers.h"

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
    window.init(640, 480, "Respite");
    world.init();


    //////// CUSTOM GAME TEST CODE ////////
    Player *p = new Player;
    p->pos.x = 2;
    p->pos.y = 2;
    world.addEntity(1, p);

    Thing *t = new Thing;
    world.addEntity(2, t);
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

    // Destroy the window manager
    window.destroy();

}