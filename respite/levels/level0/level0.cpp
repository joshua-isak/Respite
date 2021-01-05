#include <stdio.h>

#include "respite/respite.h"


void Level0::init() {

    Player *p = new Player;

    p->x = 7;
    p->y = 9;

    printf("x:%f y:%f\n", p->x, p->y);

    addEntity(p);

    Player *p2 = new Player;
    delete p2;
}
