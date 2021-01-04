#include <iostream>
#include <list>
#include <iterator>

// Platform specific
#include <Windows.h>

using namespace std;


class Entity {
public:
    float x;
    float y;
    float z;

    // Function that is run every tick
    virtual void update(){};

};


class Player: public Entity {
public:
    int count = 0;

    void update() {
        count += 1;
        printf("Count: %d", count);
    }
};



class Level {
public:
    // List of all entities in a level
    list <Entity*> entities;

    // Add an entity to the end of the entities list
    void addEntity(Entity *e) {
        entities.push_back(e);
    }
};



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

        Sleep(1/tickrate);  //expand this to account for simulation lag and render lag
    }
}