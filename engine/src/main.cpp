#include <iostream>
#include <list>
#include <iterator>

// Platform specific
#include <Windows.h>

// Engine stuff
#include "entity.h"
#include "level.h"

// Custom stuff, should ideally not be here
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


int main(int argc, char *argv[]) {
    // Engine parameters
    int tickrate = 5; // Simulation speed

    // Engine variables
    Level *current_level;    // Current level to simulate and display


    // Initialize level0 and set it as the current level
    Level0 level0;
    level0.init();
    current_level = &level0;

    // Add a player entity to the level
    // Player p;
    // (*current_level).addEntity(&p);


    printf("starting game loop\n");

    // Main game loop
    while (true) {

        //processInput();
        update(current_level);
        //render();

        Sleep(1000/tickrate);  //expand this to account for simulation lag and render lag
    }
}