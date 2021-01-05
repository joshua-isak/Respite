#include <stdio.h>

#include "entity.h"

void Entity::update(){}

void Entity::render() {
    // Render self
}

Entity::~Entity() {
    printf("bro this object was destroyed\n");
}