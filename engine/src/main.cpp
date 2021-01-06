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

// Respite game stuff, should ideally not be here
#include "respite/respite.h"


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

// Subsystem declarations
Window window;


int main(int argc, char *argv[]) {
    // Engine parameters
    int tickrate = 2;           // simulation cycles per second

    // Engine variables
    Level *current_level;       // current level to simulate and display
    bool running = true;        // current engine state (false breaks main game loop)


    // Initialize level0 and set it as the current level
    Level0 level0;
    level0.init();
    current_level = &level0;

    // Create the scene manager (replace above)


    // Inigialize subsystems
    window.init(640, 480, "Respite");



    printf("starting game loop\n");

    // Main game loop
    while (running) {

        window.processInput();
        update(current_level);
        //render();

        //expand this to account for simulation lag and render lag
        this_thread::sleep_for(chrono::milliseconds(1000/tickrate));
    }

    // Destroy the window
    window.destroy();

}