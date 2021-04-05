#include <iostream>
#include <string>
#include <unordered_map>

#include "assertions.h"
#include "cells.h"
#include "fm.h"
#include "init.h"
#include "io.h"
#include "nets.h"

int main(int argc, char const* argv[]) {
    using namespace std;

    log_release_mode();

    double balance;
    vector<Net*> net_map;
    vector<Cell*> cell_map;
    vector<string> net_names, cell_names;
    unordered_map<string, vector<string>> tmp_net_map, tmp_cell_map;

    input(argv[1], balance, tmp_net_map, tmp_cell_map);
    transformation(tmp_net_map, tmp_cell_map, net_map, cell_map, net_names,
                   cell_names);

    const unsigned net_size = tmp_net_map.size(),
                   cell_size = tmp_cell_map.size();

    assert(net_size == net_map.size());
    assert(net_size == net_names.size());
    assert(cell_size == cell_map.size());
    assert(cell_size == cell_names.size());

    const unsigned tolerate = static_cast<unsigned>(balance * cell_size);
    const unsigned half = cell_size >> 1;
    const unsigned too_much = half + tolerate;

    constexpr bool sophisticated = true;
    unsigned sideTrue(init_side(net_map, cell_map, too_much, sophisticated));

    silent_printf(
        "balance = %lf, "
        "net_size = %u, "
        "cell_size = %u\n",
        balance, net_size, cell_size);

    init_gains(net_map, cell_map);
    assert_gains(net_map, cell_map);

    fm(net_map, cell_map, tolerate, sideTrue);

    assert_gains(net_map, cell_map);

    output(argv[2], net_map, cell_map, net_names, cell_names);

    assert(net_map.size() == net_size);
    assert(cell_map.size() == cell_size);

    for (Net* net_ptr : net_map) {
        delete net_ptr;
    }

    for (Cell* cell_ptr : cell_map) {
        delete cell_ptr;
    }

    silent_printf("Program exit.\n");

    return 0;
}
