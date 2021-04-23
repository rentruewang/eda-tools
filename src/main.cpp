#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>

#include "cells.h"
#include "fm.h"
#include "nets.h"
#include "utils.h"

int main(int argc, char const* argv[]) {
    using namespace std;

    log_release_mode();
    assert(argc == 3);

    FloorPlan fp;

    fp.input(string(argv[1]));

    const unsigned nsize = fp.nmap().size();
    const unsigned csize = fp.cmap().size();
    const double bal = fp.balance();

    const unsigned tolerate = static_cast<unsigned>(bal * csize);
    fp.tolerate(tolerate);

    constexpr bool soph = true;
    fp.init_side<soph>();

    debug_printf(
        "balance = %lf, "
        "net_size = %u, "
        "cell_size = %u\n",
        bal, nsize, csize);

    fp.fm();

    fp.output(string(argv[2]));

    debug_printf("Program exit.\n");

    return 0;
}
