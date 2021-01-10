#include <stdio.h>

#include "respite/respite.h"


void Level0::init() {

    Player *p = new Player;

    p->pos.x = 7;
    p->pos.y = 9;

    printf("x:%f y:%f\n", p->pos.x, p->pos.y);

    addEntity(p);

    Player *p2 = new Player;
    delete p2;
}
