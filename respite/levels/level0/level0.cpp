#include <stdio.h>

#include "respite/respite.h"


void Level0::init() {
    // Player p;
    // addEntity(&p);
    // p.x = 7;
    // p.y = 8;

    Player *p = new Player;
    p->x = 7;
    p->y = 9;

    printf("x:%d y:%d\n", p->x, p->y);

    addEntity(p);
}
