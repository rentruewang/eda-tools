#include "nets.h"

using namespace std;

Net::Net() : _true_count(0), _cells(vector<unsigned>()) {}

void Net::setCount(unsigned u) {
    _true_count = u;
}

unsigned Net::true_count() const {
    return _true_count;
}

unsigned Net::false_count() const {
    return _cells.size() - _true_count;
}

unsigned Net::count(bool side) const {
    if (side) {
        return true_count();
    } else {
        return false_count();
    }
}

void Net::inc_count(bool side) {
    if (side) {
        ++_true_count;
    } else {
        --_true_count;
    }
}

void Net::dec_count(bool side) {
    inc_count(!side);
}

void Net::push_cell(unsigned cell) {
    _cells.push_back(cell);
}

const vector<unsigned>& Net::cells() const {
    return _cells;
}

unsigned Net::size() const {
    return _cells.size();
}
