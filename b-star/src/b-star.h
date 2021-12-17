#pragma once

#include <string>
#include <utility>
#include <vector>

#include "pins.h"

class BStar {
    std::vector<unsigned> nodes;
    std::vector<Pin>* pin_list_ptr;

   public:
    BStar();
    BStar(std::vector<Pin>& plist, unsigned width, unsigned height);

    unsigned get_size(void) const;
    unsigned get_root(void) const;

    std::vector<Pin>& pin_list(void);
    const std::vector<Pin>& pin_list(void) const;

    void check(void) const;
    std::pair<int, int> update(void) const;

    void flip(void);

    void permute(unsigned idx);
    void swap(unsigned i, unsigned j);
    void delete_insert(unsigned from,
                       unsigned to,
                       bool from_side,
                       bool to_side);
    void mirror(unsigned i);

    unsigned random_permute(void);
    std::pair<unsigned, unsigned> random_swap(void);
    std::pair<std::pair<unsigned, bool>, std::pair<unsigned, bool>>
    random_delete_insert(void);
    unsigned random_mirror(void);

    void revert_permute(unsigned action);
    void revert_swap(std::pair<unsigned, unsigned> action);
    void revert_delete_insert(
        std::pair<std::pair<unsigned, bool>, std::pair<unsigned, bool>> action);
    void revert_mirror(unsigned action);
};
