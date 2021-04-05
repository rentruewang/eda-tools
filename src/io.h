#pragma once

#include <cstdarg>
#include <string>
#include <unordered_map>
#include <vector>

#include "assertions.h"
#include "cells.h"
#include "nets.h"

void input(const char* name,
           double& balance,
           std::unordered_map<std::string, std::vector<std::string>>& nmap,
           std::unordered_map<std::string, std::vector<std::string>>& cmap);

void transformation(
    std::unordered_map<std::string, std::vector<std::string>>& old_nmap,
    std::unordered_map<std::string, std::vector<std::string>>& old_cmap,
    std::vector<Net*>& nmap,
    std::vector<Cell*>& cmap,
    std::vector<std::string>& nnames,
    std::vector<std::string>& cnames);

void output(const char* name,
            const std::vector<Net*>& nmap,
            const std::vector<Cell*>& cmap,
            const std::vector<std::string>& nnames,
            const std::vector<std::string>& cnames);

int silent_printf(const char* format, ...);
