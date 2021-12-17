#include "fm.h"

using namespace std;

FloorPlan::FloorPlan() : FloorPlan(vector<Net*>(), vector<Cell*>()) {}

FloorPlan::FloorPlan(vector<Net*>& n, vector<Cell*>& c)
    : _nmap(n),
      _cmap(c),
      _bucket(),
      _balance(0.),
      _tcount((unsigned)-1),
      _tolerate((unsigned)-1) {}

FloorPlan::FloorPlan(vector<Net*>&& n, vector<Cell*>&& c)
    : _nmap(move(n)),
      _cmap(move(c)),
      _balance(0.),
      _tcount((unsigned)-1),
      _tolerate((unsigned)-1) {
    _bucket = move(Bucket(_cmap));
}

FloorPlan::~FloorPlan() {
    for (Net* nptr : _nmap) {
        delete nptr;
    }

    for (Cell* cptr : _cmap) {
        delete cptr;
    }
}

void FloorPlan::tolerate(unsigned amount) {
    _tolerate = amount;
}

void FloorPlan::nmap(vector<Net*>&& nmap) {
    this->_nmap = nmap;
}

vector<Net*>& FloorPlan::nmap() {
    return _nmap;
}

const vector<Net*>& FloorPlan::nmap() const {
    return _nmap;
}

void FloorPlan::cmap(vector<Cell*>&& cmap) {
    this->_cmap = cmap;
}

vector<Cell*>& FloorPlan::cmap() {
    return _cmap;
}

const vector<Cell*>& FloorPlan::cmap() const {
    return _cmap;
}

double FloorPlan::balance() const {
    return _balance;
}
