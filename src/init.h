#pragma once

#include <vector>

#include "assertions.h"
#include "cells.h"
#include "nets.h"

unsigned naive_init_side(std::vector<Cell*>& cmap);

unsigned sophisticated_init_side(std::vector<Net*>& nmap,
                                 std::vector<Cell*>& cmap,
                                 const unsigned too_much);

unsigned init_side(std::vector<Net*>& nmap,
                   std::vector<Cell*>& cmap,
                   const unsigned too_much,
                   const bool sophisticated);

template <bool checking>
void gains(std::vector<Net*>& nmap, std::vector<Cell*>& cmap);

void init_gains(std::vector<Net*>& nmap, std::vector<Cell*>& cmap);
void assert_gains(std::vector<Net*>& nmap, std::vector<Cell*>& cmap);
