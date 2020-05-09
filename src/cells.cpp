#include "cells.h"

using namespace std;

Cell::Cell() : side(0), gain(0), nets(vector<unsigned>()) {}

void Cell::flip() {
    side = !side;
}

void Cell::setSide(bool s) {
    side = static_cast<int>(s);
}

bool Cell::getSide() const {
    return static_cast<bool>(side);
}

int Cell::getGain() const {
    return gain;
}

void Cell::setGain(int g) {
    gain = g;
}

void Cell::incGain() {
    ++gain;
}

void Cell::decGain() {
    --gain;
}

void Cell::pushNet(unsigned net) {
    nets.push_back(net);
}

const vector<unsigned>& Cell::getNets() const {
    return nets;
}

unsigned Cell::size() const {
    return nets.size();
}
