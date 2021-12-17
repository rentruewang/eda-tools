#pragma once

#include <utility>

#include "b-star.h"
#include "nets.h"
#include "pins.h"

unsigned total_area(const std::pair<int, int> dim);

unsigned total_hpwl(const std::vector<Pin>& pin_list,
                    const std::vector<Net>& net_list);

double total_cost(const double area, const double hpwl, const double alpha);

std::pair<int, int> SA(std::pair<unsigned, unsigned> boundary,
                       BStar& tree,
                       std::vector<Pin>& pin_list,
                       const std::vector<Net>& net_list,
                       std::pair<unsigned, unsigned> iter_info,
                       unsigned num_blocks,
                       unsigned episodes,
                       unsigned high_temp,
                       double alpha,
                       double ratio,
                       double P,
                       double C);
