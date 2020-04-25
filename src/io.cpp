#include "io.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

using namespace std;

void input(const char *name, double &balance,
           unordered_map<string, vector<string>> &net_map,
           unordered_map<string, vector<string>> &cell_map) {
    using namespace std;

    unordered_map<string, unordered_set<string>> net_map_set, cell_map_set;

    auto file = ifstream(name);

    file >> balance;

    string buffer;
    while (file >> buffer) {
        if (buffer != "NET") break;

        string net_name;
        file >> net_name;

        net_map_set[net_name] = unordered_set<string>();

        unordered_set<string> &current_net = net_map_set[net_name];

        string cell_name;
        while (file >> cell_name) {
            if (cell_name == ";") break;

            if (cell_map_set.find(cell_name) == cell_map_set.end()) {
                cell_map_set[cell_name] = unordered_set<string>();
            }

            current_net.insert(cell_name);

            unordered_set<string> &current_cell = cell_map_set[cell_name];
            current_cell.insert(net_name);
        }
    }

    assert(net_map.size() == 0);
    assert(cell_map.size() == 0);

    for (auto iter = net_map_set.begin(); iter != net_map_set.end(); ++iter) {
        const unordered_set<string> &names = iter->second;
        net_map[iter->first] = vector<string>(names.begin(), names.end());
    }
    for (auto iter = cell_map_set.begin(); iter != cell_map_set.end(); ++iter) {
        const unordered_set<string> &names = iter->second;
        cell_map[iter->first] = vector<string>(names.begin(), names.end());
    }

    assert(net_map.size() == net_map_set.size());
    assert(cell_map.size() == cell_map_set.size());
}

void transformation(unordered_map<string, vector<string>> &old_net_map,
                    unordered_map<string, vector<string>> &old_cell_map,
                    vector<Net *> &net_map, vector<Cell *> &cell_map,
                    vector<string> &net_names, vector<string> &cell_names) {
    using namespace std;

    unordered_map<string, unsigned> rev_net_names, rev_cell_names;

    assert(net_map.size() == 0 && cell_map.size() == 0);
    assert(net_names.size() == 0 && cell_names.size() == 0);

    net_map.reserve(old_net_map.size());
    cell_map.reserve(old_cell_map.size());

    net_names.reserve(old_net_map.size());
    cell_names.reserve(old_cell_map.size());

    for (unsigned idx = 0; idx < old_net_map.size(); ++idx) {
        net_map.push_back(new Net());
    }
    for (unsigned idx = 0; idx < old_cell_map.size(); ++idx) {
        cell_map.push_back(new Cell());
    }

    for (auto iter = old_net_map.begin(); iter != old_net_map.end(); ++iter) {
        const string name = iter->first;
        if (rev_net_names.find(name) == rev_net_names.end()) {
            rev_net_names[name] = net_names.size();
            net_names.push_back(name);
        }
    }

    for (auto iter = old_cell_map.begin(); iter != old_cell_map.end(); ++iter) {
        const string name = iter->first;
        if (rev_cell_names.find(name) == rev_cell_names.end()) {
            rev_cell_names[name] = cell_names.size();
            cell_names.push_back(name);
        }
    }

    for (auto iter = old_net_map.begin(); iter != old_net_map.end(); ++iter) {
        const string nname = iter->first;
        const vector<string> &cell_list = iter->second;
        const unsigned net_id = rev_net_names[nname];

        Net *net = net_map[net_id];

        assert(net_id < net_map.size());

        for (unsigned idx = 0; idx < cell_list.size(); ++idx) {
            const string cname = cell_list[idx];
            const unsigned cell_id = rev_cell_names[cname];

            assert(cell_id < cell_map.size());

            net->pushCell(cell_id);

            Cell *cell = cell_map[cell_id];
            cell->pushNet(net_id);
        }
    }

    // Asserts
    assert(net_names.size() == rev_net_names.size());
    for (unsigned idx = 0; idx < net_names.size(); ++idx) {
        const string name = net_names[idx];
        assert(rev_net_names[name] == idx);
    }
    assert(cell_names.size() == rev_cell_names.size());
    for (unsigned idx = 0; idx < cell_names.size(); ++idx) {
        const string name = cell_names[idx];
        assert(rev_cell_names[name] == idx);
    }
}

void output(const char *name, const vector<Net *> &net_map,
            const vector<Cell *> &cell_map, const vector<string> &net_names,
            const vector<string> &cell_names) {
    using namespace std;

    stringstream ss;
    auto file = ofstream(name);

    unsigned cut_size = 0;
    for (unsigned idx = 0; idx < net_map.size(); ++idx) {
        const Net *net = net_map[idx];
        if (net->trueCount() && net->falseCount()) ++cut_size;
    }

    ss << "Cutsize = " << cut_size << "\n";
    file << ss.str();

    stringstream true_ss, false_ss;
    unsigned true_count = 0, false_count = 0;

    true_ss << "\n";
    false_ss << "\n";

    for (unsigned idx = 0; idx < cell_map.size(); ++idx) {
        const string name = cell_names[idx];
        const Cell *cell = cell_map[idx];
        if (cell->getSide()) {
            ++true_count;
            true_ss << name << " ";
        } else {
            ++false_count;
            false_ss << name << " ";
        }
    }

    true_ss << ";\n";
    false_ss << ";\n";

    assert(true_count + false_count == cell_map.size());

    file << "G1 " << true_count << true_ss.str();
    file << "G2 " << false_count << false_ss.str();
}
