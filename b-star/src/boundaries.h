#pragma once

#include "pins.h"

class Boundary {
    int r, y;

   public:
    Boundary(int r, int y);
    Boundary(const Pin& pin);
    Boundary(const Boundary& bnd);
    Boundary(Boundary&& bnd);

    Boundary& operator=(const Boundary& bnd);
    Boundary& operator=(Boundary&& bnd);

    bool operator==(const Boundary& bnd);

    int right() const;
    int top() const;

    void set_right(int r);
    void set_top(int y);
};
