#include "nets.h"

using namespace std;

Net::Net() : on_true_side(0), cells(vector<unsigned>()) {}

void Net::setCount(unsigned u) {
    on_true_side = u;
}

unsigned Net::trueCount() const {
    return on_true_side;
}

unsigned Net::falseCount() const {
    return cells.size() - on_true_side;
}

unsigned Net::countOn(bool side) const {
    if (side) {
        return trueCount();
    } else {
        return falseCount();
    }
}

void Net::incCountOn(bool side) {
    if (side) {
        ++on_true_side;
    } else {
        --on_true_side;
    }
}

void Net::decCountOn(bool side) {
    incCountOn(!side);
}

void Net::pushCell(unsigned cell) {
    cells.push_back(cell);
}

const vector<unsigned>& Net::getCells() const {
    return cells;
}

unsigned Net::size() const {
    return cells.size();
}
