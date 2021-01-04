#include <stdio.h>

#include "respite/respite.h"


void Level0::init() {
    Player p;
    addEntity(&p);
    p.x = 7;
    p.y = 8;
}
