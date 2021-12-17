#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "b-star.h"
#include "io.h"
#include "nets.h"
#include "pins.h"
#include "sa.h"

int main(int argc, char const* argv[]) {
    using namespace std;

    time_t start = time(NULL);

    srand(start);

    printf("start time = %ld\n", start);

    double alpha = atof(argv[1]);

    printf("alpha = %lf\n", alpha);

    auto pin_map = unordered_map<string, unsigned>();
    auto pin_list = vector<Pin>();
    auto net_list = vector<Net>();

    auto pin_file = ifstream(argv[2]);
    auto net_file = ifstream(argv[3]);

    unsigned num_blocks;

    auto dimension = read_pin_file(pin_file, pin_list, pin_map, num_blocks);
    read_net_file(net_file, pin_list, net_list, pin_map);

    unsigned width = dimension.first;
    unsigned height = dimension.second;

    printf("blocks = %lu, nets = %lu, dimension= (%u, %u)\n", pin_map.size(),
           net_list.size(), width, height);

    BStar tree = BStar(pin_list, width, height);

    constexpr double init_prob = .999, constant = 3.;
    constexpr unsigned iterations = 10000, interrupt = iterations / constant;
    constexpr auto iter_info = make_pair(iterations, interrupt);
    constexpr unsigned episodes = 1000, burning_stage = 100;
    constexpr double ratio = .5;

    // static_assert(init_prob <= 1. - 1. / (double)episodes);

    auto floorplan =
        SA(dimension, tree, pin_list, net_list, iter_info, num_blocks, episodes,
           burning_stage, alpha, ratio, init_prob, constant);

    printf("result = (%d, %d)\n", floorplan.first, floorplan.second);

    auto out_file = ofstream(argv[4]);
    save_file(out_file, start, alpha, floorplan, pin_list, net_list);

    printf("Program Exit.\n");

    return 0;
}
