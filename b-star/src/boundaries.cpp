#include "boundaries.h"

Boundary::Boundary(int r, int y) : r(r), y(y) {}

Boundary::Boundary(const Pin& pin)
    : r(pin.get_xpos() + pin.get_width()),
      y(pin.get_ypos() + pin.get_height()) {}

Boundary::Boundary(const Boundary& bnd) : r(bnd.r), y(bnd.y) {}

Boundary::Boundary(Boundary&& bnd) : r(bnd.r), y(bnd.y) {
    bnd.r = bnd.y = 0;
}

Boundary& Boundary::operator=(const Boundary& bnd) {
    r = bnd.r;
    y = bnd.y;

    return *this;
}

Boundary& Boundary::operator=(Boundary&& bnd) {
    r = bnd.r;
    y = bnd.y;

    bnd.r = bnd.y = 0;

    return *this;
}

bool Boundary::operator==(const Boundary& bnd) {
    return r == bnd.r && y == bnd.y;
}

int Boundary::right() const {
    return r;
}

int Boundary::top() const {
    return y;
}

void Boundary::set_right(int r) {
    this->r = r;
}

void Boundary::set_top(int y) {
    this->y = y;
}
