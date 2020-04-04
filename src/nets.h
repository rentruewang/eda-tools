#ifndef NETS_H
#define NETS_H

#include <vector>

class Net {
    unsigned on_true_side;
    std::vector<unsigned> cells;

   public:
    Net() : on_true_side(0), cells(std::vector<unsigned>()) {}

    void setCount(unsigned u) { on_true_side = u; }

    unsigned trueCount() const { return on_true_side; }
    unsigned falseCount() const { return cells.size() - on_true_side; }
    unsigned countOn(bool side) const {
        if (side) {
            return trueCount();
        } else {
            return falseCount();
        }
    }
    void incCountOn(bool side) {
        if (side) {
            ++on_true_side;
        } else {
            --on_true_side;
        }
    }
    void decCountOn(bool side) { incCountOn(!side); }

    void pushCell(unsigned cell) { cells.push_back(cell); }

    const std::vector<unsigned> &getCells() const { return cells; }

    unsigned size() const { return cells.size(); }
};

#endif