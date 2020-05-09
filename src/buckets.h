#pragma once

#include <map>
#include <unordered_set>

#include "assertions.h"
#include "cells.h"

class List {
    std::unordered_set<unsigned> cells;

   public:
    List();
    const std::unordered_set<unsigned>& getList() const;
    unsigned size() const;
    void push(unsigned name);
    unsigned pop();
    bool contains(const unsigned name) const;
    void erase(const unsigned name);
};

class Bucket {
    std::map<int, List> bucket;

   public:
    Bucket();
    Bucket(const std::vector<Cell*>& cell_map);

    const std::map<int, List>& getBucket() const;

    void push(const unsigned name, const Cell* cell);
    unsigned pop();
    unsigned size() const;

    bool contains(unsigned name);
    void update(int old_gain, int new_gain, unsigned name);
    void fill(const std::vector<Cell*>& cell_map);
    void empty(Bucket& other, std::unordered_set<unsigned>& seen);

    Bucket& operator=(Bucket&& b);
};
