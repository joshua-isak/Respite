#pragma once

#include <tuple>

// A point in 3D space
struct point {
    float x = 0;
    float y = 0;
    float z = 0;


    bool operator<(const point& coord) const {
        if(x < coord.x) return true;
        if(x > coord.x) return false;
        //x == coord.x
        if(y < coord.y) return true;
        if(y > coord.y) return false;
        //x == coord.x && y == coord.y
        if(z < coord.z) return true;
        if(z > coord.z) return false;
        //*this == coord
        return false;
    }
};


// A rectuangular prism defined by 2 points in 3D space
struct box {
    point p_highest;
    point p_lowest;
};