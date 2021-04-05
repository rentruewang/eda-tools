#include "io.h"

#include <cstdarg>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

using namespace std;

void input(const char* name,
           double& balance,
           unordered_map<string, vector<string>>& nmap,
           unordered_map<string, vector<string>>& cmap) {
    unordered_map<string, unordered_set<string>> nmap_set, cmap_set;

    auto file = ifstream(name);

    file >> balance;

    string buffer;
    while (file >> buffer) {
        if (buffer != "NET") {
            break;
        }

        string nname;
        file >> nname;

        nmap_set[nname] = unordered_set<string>();

        unordered_set<string>& current_net = nmap_set[nname];

        string cname;
        while (file >> cname) {
            if (cname == ";") {
                break;
            }

            if (!cmap_set.contains(cname)) {
                cmap_set[cname] = unordered_set<string>();
            }

            current_net.insert(cname);

            unordered_set<string>& current_cell = cmap_set[cname];
            current_cell.insert(nname);
        }
    }

    assert(nmap.size() == 0);
    assert(cmap.size() == 0);

    for (auto iter = nmap_set.begin(); iter != nmap_set.end(); ++iter) {
        const auto& names = iter->second;
        nmap[iter->first] = vector<string>(names.begin(), names.end());
    }
    for (auto iter = cmap_set.begin(); iter != cmap_set.end(); ++iter) {
        const auto& names = iter->second;
        cmap[iter->first] = vector<string>(names.begin(), names.end());
    }

    assert(nmap.size() == nmap_set.size());
    assert(cmap.size() == cmap_set.size());
}

void transformation(unordered_map<string, vector<string>>& old_nmap,
                    unordered_map<string, vector<string>>& old_cmap,
                    vector<Net*>& nmap,
                    vector<Cell*>& cmap,
                    vector<string>& nnames,
                    vector<string>& cnames) {
    unordered_map<string, unsigned> rev_nnames, rev_cnames;

    assert(nmap.size() == 0 && cmap.size() == 0);
    assert(nnames.size() == 0 && cnames.size() == 0);

    nmap.reserve(old_nmap.size());
    cmap.reserve(old_cmap.size());

    nnames.reserve(old_nmap.size());
    cnames.reserve(old_cmap.size());

    for (unsigned idx = 0; idx < old_nmap.size(); ++idx) {
        nmap.push_back(new Net());
    }
    for (unsigned idx = 0; idx < old_cmap.size(); ++idx) {
        cmap.push_back(new Cell());
    }

    for (auto iter = old_nmap.begin(); iter != old_nmap.end(); ++iter) {
        const string name = iter->first;
        if (!rev_nnames.contains(name)) {
            rev_nnames[name] = nnames.size();
            nnames.push_back(name);
        }
    }

    for (auto iter = old_cmap.begin(); iter != old_cmap.end(); ++iter) {
        const string name = iter->first;
        if (!rev_cnames.contains(name)) {
            rev_cnames[name] = cnames.size();
            cnames.push_back(name);
        }
    }

    for (auto iter = old_nmap.begin(); iter != old_nmap.end(); ++iter) {
        const string nname = iter->first;
        const vector<string>& cell_list = iter->second;
        const unsigned net_id = rev_nnames[nname];

        Net* net = nmap[net_id];

        assert(net_id < nmap.size());

        for (unsigned idx = 0; idx < cell_list.size(); ++idx) {
            const string cname = cell_list[idx];
            const unsigned cell_id = rev_cnames[cname];

            assert(cell_id < cmap.size());

            net->pushCell(cell_id);

            Cell* cell = cmap[cell_id];
            cell->pushNet(net_id);
        }
    }

    // Asserts
    assert(nnames.size() == rev_nnames.size());
    for (unsigned idx = 0; idx < nnames.size(); ++idx) {
        const string name = nnames[idx];
        assert(rev_nnames[name] == idx);
    }
    assert(cnames.size() == rev_cnames.size());
    for (unsigned idx = 0; idx < cnames.size(); ++idx) {
        const string name = cnames[idx];
        assert(rev_cnames[name] == idx);
    }
}

void output(const char* name,
            const vector<Net*>& nmap,
            const vector<Cell*>& cmap,
            const vector<string>& nnames,
            const vector<string>& cnames) {
    stringstream ss;
    auto file = ofstream(name);

    unsigned cut_size = 0;
    for (unsigned idx = 0; idx < nmap.size(); ++idx) {
        const Net* net = nmap[idx];
        if (net->trueCount() && net->falseCount()) {
            ++cut_size;
        }
    }

    ss << "Cutsize = " << cut_size << "\n";
    file << ss.str();

    stringstream true_ss, false_ss;
    unsigned true_count = 0, false_count = 0;

    true_ss << "\n";
    false_ss << "\n";

    for (unsigned idx = 0; idx < cmap.size(); ++idx) {
        const string name = cnames[idx];
        const Cell* cell = cmap[idx];
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

    assert(true_count + false_count == cmap.size());

    file << "G1 " << true_count << true_ss.str();
    file << "G2 " << false_count << false_ss.str();
}

int silent_printf(const char* format, ...) {
#if !defined(SILENT)
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    return result;
#endif
    return 0;
}
