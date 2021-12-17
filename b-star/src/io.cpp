#include "io.h"

#include <assert.h>
#include <time.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "costs.h"
#include "sa.h"

using namespace std;

pair<unsigned, unsigned> read_pin_file(ifstream& file,
                                       vector<Pin>& pin_list,
                                       unordered_map<string, unsigned>& pin_map,
                                       unsigned& num_blocks) {
    auto buffer = string();

    unsigned out_width, out_height;

    file >> buffer >> out_width >> out_height;
    assert(buffer == "Outline:");

    file >> buffer >> num_blocks;
    assert(buffer == "NumBlocks:");

    unsigned num_terminals;
    file >> buffer >> num_terminals;
    assert(buffer == "NumTerminals:");

    auto name = string();

    unsigned width, height;
    for (unsigned i = 0; i < num_blocks; ++i) {
        file >> name >> width >> height;
        pin_map[name] = pin_list.size();
        pin_list.push_back(Pin(width, height, true, name));
    }
    assert(pin_map.size() == num_blocks);

    unsigned horiz, verti;
    for (unsigned i = 0; i < num_terminals; ++i) {
        file >> name >> buffer >> horiz >> verti;
        assert(buffer == "terminal");
        pin_map[name] = pin_list.size();
        pin_list.push_back(Pin(horiz, verti, false, name));
    }
    assert(pin_map.size() == num_blocks + num_terminals);
    assert(pin_list.size() == num_blocks + num_terminals);

    for (int i = 0; i < (int)num_blocks; ++i) {
        assert(pin_list[i].area_nonzero());
    }

    for (int i = num_blocks; (unsigned)i < num_blocks + num_terminals; ++i) {
        assert(!pin_list[i].area_nonzero());
    }

    return make_pair(out_width, out_height);
}

void read_net_file(ifstream& file,
                   const vector<Pin>& pin_list,
                   vector<Net>& net_list,
                   const unordered_map<string, unsigned>& pin_map) {
    auto buffer = string();

    unsigned num_nets;

    file >> buffer >> num_nets;
    assert(buffer == "NumNets:");

    for (unsigned i = 0, num_degree; i < num_nets; ++i) {
        file >> buffer >> num_degree;
        assert(buffer == "NetDegree:");
        auto connected = vector<unsigned>();
        connected.reserve(num_degree);
        for (unsigned j = 0; j < num_degree; ++j) {
            auto pin_name = string();
            file >> pin_name;
            connected.push_back(pin_map.at(pin_name));
        }
        sort(connected.rbegin(), connected.rend(), [&](int i, int j) {
            return pin_list[connected[i]].area_nonzero() <
                   pin_list[connected[j]].area_nonzero();
        });
        assert(connected.size() == num_degree);
        net_list.push_back(move(Net(move(connected), pin_list)));
    }
    assert(net_list.size() == num_nets);
}

void save_file(ofstream& file,
               time_t start_time,
               double alpha,
               pair<int, int> dimension,
               const vector<Pin>& pin_list,
               const vector<Net>& net_list) {
    const unsigned size = pin_list.size();

    const unsigned width = dimension.first, height = dimension.second;
    const unsigned area = total_area(dimension);
    const unsigned hpwl = total_hpwl(pin_list, net_list);
    const double cost = total_cost(area, hpwl, alpha);

    file << cost << "\n"
         << hpwl << "\n"
         << area << "\n"
         << width << " " << height << "\n"
         << difftime(time(NULL), start_time) << "\n";

    for (unsigned i = 0; i < size; ++i) {
        const Pin& pin = pin_list[i];
        const string& name = pin.get_name();

        if (pin.area_nonzero()) {
            const int x = pin.get_xpos(), y = pin.get_ypos(),
                      w = pin.get_width(), h = pin.get_height();

            file << name << " " << x << " " << y << " " << x + w << " " << y + h
                 << "\n";
        }
    }
}
