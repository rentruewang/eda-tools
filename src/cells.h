#pragma once

#include <memory>
#include <vector>

class Cell {
    // using int to prevent padding issue
    int side;
    int gain;
    std::vector<unsigned> nets;

   public:
    Cell();
    void flip();

    void setSide(bool s);
    bool getSide() const;

    int getGain() const;
    void setGain(int g);
    void incGain();
    void decGain();

    void pushNet(unsigned net);
    const std::vector<unsigned> &getNets() const;
    unsigned size() const;
};
