#ifndef CELLS_H
#define CELLS_H

#include <memory>
#include <vector>

class Cell {
    // using int to prevent padding issue
    int side;
    int gain;
    std::vector<unsigned> nets;

   public:
    Cell() : side(0), gain(0), nets(std::vector<unsigned>()) {}
    void flip() { side = !side; }

    void setSide(bool s) { side = static_cast<int>(s); }
    bool getSide() const { return static_cast<bool>(side); }

    int getGain() const { return gain; }
    void setGain(int g) { gain = g; }
    void incGain() { ++gain; }
    void decGain() { --gain; }

    void pushNet(unsigned net) { nets.push_back(net); }
    const std::vector<unsigned> &getNets() const { return nets; }

    unsigned size() const { return nets.size(); }
};

#endif
