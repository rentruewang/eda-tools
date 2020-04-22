#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "assertions.h"
#include "buckets.h"
#include "cells.h"
#include "nets.h"

struct Record {
    int before;
    int after;
    Record();
    Record(int b, int a);
};

bool meta_data(unsigned num_iter, int result);

void modify_gain(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
                 std::unordered_map<unsigned, Record> &records,
                 const unsigned name, const bool increase);

int flip_cell(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
              const unsigned cell_name, Cell *cell,
              std::unordered_map<unsigned, Record> &records);

int wrap_flip_cell(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
                   Bucket &bucket, Bucket &next_bucket,
                   const std::unordered_set<unsigned> &seen,
                   const unsigned cell_name, Cell *cell);

int fm_once(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
            Bucket &bucket, std::function<bool(const unsigned)> condition,
            unsigned &partition);

void fm(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
        const unsigned tolerate, unsigned &part);