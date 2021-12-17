#pragma once

#include <vector>

class Cell {
   public:
    Cell();

    void flip();

    void side(bool s);
    bool side() const;

    int gain() const;
    void gain(int g);
    void inc_gain();
    void dec_gain();

    void push_net(unsigned net);
    const std::vector<unsigned>& nets() const;
    unsigned size() const;

   private:
    // using int to prevent padding issue
    int _side;
    int _gain;
    std::vector<unsigned> _nets;
};
