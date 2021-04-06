#pragma once

#include <vector>

class Net {
   public:
    Net();

    void setCount(unsigned u);

    unsigned true_count() const;
    unsigned false_count() const;
    unsigned count(bool side) const;
    void inc_count(bool side);
    void dec_count(bool side);

    void push_cell(unsigned cell);
    const std::vector<unsigned>& cells() const;
    unsigned size() const;

   private:
    unsigned _true_count;
    std::vector<unsigned> _cells;
};
