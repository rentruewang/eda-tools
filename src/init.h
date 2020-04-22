#pragma once

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "assertions.h"
#include "cells.h"
#include "nets.h"

unsigned naive_init_side(std::vector<Cell *> &cell_map);

unsigned sophisticated_init_side(std::vector<Net *> &net_map,
                                 std::vector<Cell *> &cell_map,
                                 const unsigned too_much);

unsigned init_side(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
                   const unsigned too_much, const bool sophisticated);

void gains(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
           const bool checking);

void init_gains(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map);
void assert_gains(std::vector<Net *> &net_map, std::vector<Cell *> &cell_map);
