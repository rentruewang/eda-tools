#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "assertions.h"
#include "buckets.h"
#include "cells.h"
#include "nets.h"

bool meta_data(unsigned num_iter, int result);

void fm(std::vector<Net*>& nmap,
        std::vector<Cell*>& cmap,
        const unsigned tolerate,
        unsigned& part);
