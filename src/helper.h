#pragma once

#include <unordered_map>
#include <vector>

#include "buckets.h"
#include "cells.h"
#include "nets.h"

enum class Mod { Inc, Dec };

struct GainChange {
    int original;
    int updated;

    GainChange() : GainChange(0, 0) {}
    GainChange(int o, int u) : original(o), updated(u) {}
};

template <Mod mod>
void store_updates(std::vector<Cell*>& cmap,
                   std::unordered_map<unsigned, GainChange>& records,
                   const unsigned name);

int flip_cell(std::vector<Net*>& nmap,
              std::vector<Cell*>& cmap,
              const unsigned cname,
              Cell* cell,
              std::unordered_map<unsigned, GainChange>& records);

int flip(std::vector<Net*>& nmap,
         std::vector<Cell*>& cmap,
         Bucket& bucket,
         Bucket& nbucket,
         const std::unordered_set<unsigned>& seen,
         const unsigned cname,
         Cell* cell);
