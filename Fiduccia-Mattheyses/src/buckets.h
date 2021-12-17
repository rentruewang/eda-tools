#pragma once

#include <map>
#include <unordered_set>

#include "cells.h"

class List {
   public:
    List();
    const std::unordered_set<unsigned>& get() const;
    unsigned size() const;
    void push(unsigned name);
    unsigned pop();
    bool contains(const unsigned name) const;
    void erase(const unsigned name);

   private:
    std::unordered_set<unsigned> cells;
};

class Bucket {
   public:
    Bucket();
    Bucket(const std::vector<Cell*>& cmap);

    Bucket& operator=(Bucket&& b);

    const std::map<int, List>& get() const;

    void push(const unsigned name, const Cell* cell);
    unsigned pop();
    unsigned size() const;

    bool contains(unsigned name);
    void update(int old_gain, int new_gain, unsigned name);
    void fill(const std::vector<Cell*>& cmap);
    void empty(Bucket& other, std::unordered_set<unsigned>& seen);

   private:
    std::map<int, List> bucket;
};
