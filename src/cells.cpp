#include "cells.h"

using namespace std;

Cell::Cell() : _side(0), _gain(0), _nets(vector<unsigned>()) {}

void Cell::flip() {
    _side = !_side;
}

void Cell::side(bool s) {
    _side = static_cast<int>(s);
}

bool Cell::side() const {
    return static_cast<bool>(_side);
}

int Cell::gain() const {
    return _gain;
}

void Cell::gain(int g) {
    _gain = g;
}

void Cell::inc_gain() {
    ++_gain;
}

void Cell::dec_gain() {
    --_gain;
}

void Cell::push_net(unsigned net) {
    _nets.push_back(net);
}

const vector<unsigned>& Cell::nets() const {
    return _nets;
}

unsigned Cell::size() const {
    return _nets.size();
}
