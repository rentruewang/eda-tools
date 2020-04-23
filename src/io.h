#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "assertions.h"
#include "cells.h"
#include "nets.h"

void input(const char *name, double &balance,
           std::unordered_map<std::string, std::vector<std::string>> &net_map,
           std::unordered_map<std::string, std::vector<std::string>> &cell_map);

void transformation(
    std::unordered_map<std::string, std::vector<std::string>> &old_net_map,
    std::unordered_map<std::string, std::vector<std::string>> &old_cell_map,
    std::vector<Net *> &net_map, std::vector<Cell *> &cell_map,
    std::vector<std::string> &net_names, std::vector<std::string> &cell_names);

void output(const char *name, const std::vector<Net *> &net_map,
            const std::vector<Cell *> &cell_map,
            const std::vector<std::string> &net_names,
            const std::vector<std::string> &cell_names);
