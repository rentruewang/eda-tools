#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include "b-star.h"
#include "costs.h"
#include "nets.h"
#include "pins.h"

std::pair<unsigned, unsigned> read_pin_file(
    std::ifstream& file,
    std::vector<Pin>& pin_list,
    std::unordered_map<std::string, unsigned>& pin_map,
    unsigned& num_blocks);

void read_net_file(std::ifstream& file,
                   const std::vector<Pin>& pin_list,
                   std::vector<Net>& net_list,
                   const std::unordered_map<std::string, unsigned>& pin_map);

void save_file(std::ofstream& file,
               time_t start_time,
               double alpha,
               std::pair<int, int> dimension,
               const std::vector<Pin>& pin_list,
               const std::vector<Net>& net_list);
