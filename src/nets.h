#pragma once

#include <vector>

class Net {
    unsigned on_true_side;
    std::vector<unsigned> cells;

   public:
    Net();

    void setCount(unsigned u);

    unsigned trueCount() const;
    unsigned falseCount() const;
    unsigned countOn(bool side) const;
    void incCountOn(bool side);
    void decCountOn(bool side);

    void pushCell(unsigned cell);
    const std::vector<unsigned>& getCells() const;
    unsigned size() const;
};
