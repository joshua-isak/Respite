#include "engine/src/level.h"

#include "respite/entities/entities.h"

class Level0 : public Level {
public:

    void init() {
        Player p;
        addEntity(&p);
    }
};