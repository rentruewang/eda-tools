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
    assert(argc == 3);

    double balance;
    vector<Net*> nmap;
    vector<Cell*> cmap;
    vector<string> nnames, cnames;
    unordered_map<string, vector<string>> tmp_nmap, tmp_cmap;

    input(argv[1], balance, tmp_nmap, tmp_cmap);
    transformation(tmp_nmap, tmp_cmap, nmap, cmap, nnames, cnames);

    const unsigned nsize = tmp_nmap.size(), csize = tmp_cmap.size();

    assert(nsize == nmap.size());
    assert(nsize == nnames.size());
    assert(csize == cmap.size());
    assert(csize == cnames.size());

    const unsigned tolerate = static_cast<unsigned>(balance * csize);
    const unsigned half = csize >> 1;
    const unsigned too_much = half + tolerate;

    constexpr bool soph = true;
    unsigned sideTrue(init_side(nmap, cmap, too_much, soph));

    silent_printf(
        "balance = %lf, "
        "net_size = %u, "
        "cell_size = %u\n",
        balance, nsize, csize);

    init_gains(nmap, cmap);
    assert_gains(nmap, cmap);

    fm(nmap, cmap, tolerate, sideTrue);

    assert_gains(nmap, cmap);

    output(argv[2], nmap, cmap, nnames, cnames);

    assert(nmap.size() == nsize);
    assert(cmap.size() == csize);

    for (Net* net_ptr : nmap) {
        delete net_ptr;
    }

    for (Cell* cell_ptr : cmap) {
        delete cell_ptr;
    }

    silent_printf("Program exit.\n");

    return 0;
}
