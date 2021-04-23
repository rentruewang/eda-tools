#include "helper.h"

#include <cassert>
#include <unordered_map>
#include <vector>

#include "cells.h"
#include "fm.h"
#include "nets.h"
#include "utils.h"

using namespace std;

template <Mod mod>
void store_updates(vector<Cell*>& cmap,
                   unordered_map<unsigned, GainChange>& records,
                   unsigned name) {
    Cell* cell = cmap[name];
    int old_gain = cell->gain();
    int new_gain;
    switch (mod) {
        case Mod::Inc:
            new_gain = old_gain + 1;
            cell->inc_gain();
            break;
        case Mod::Dec:
            new_gain = old_gain - 1;
            cell->dec_gain();
            break;
    }
    assert(new_gain == cell->gain());
    if (records.contains(name)) {
        auto& record = records[name];
        assert(old_gain == record.updated);
        record.updated = new_gain;
    } else {
        records[name] = GainChange(old_gain, new_gain);
    }
}

int flip_cell(vector<Net*>& nmap,
              vector<Cell*>& cmap,
              unsigned cname,
              unordered_map<unsigned, GainChange>& records) {
    assert(records.size() == 0);

    Cell* cell = cmap[cname];

    int cutsize_reduction = 0;

    const vector<unsigned>& nets = cell->nets();
    const bool fromSide = cell->side();

    for (unsigned idx = 0; idx < nets.size(); ++idx) {
        const unsigned nname = nets[idx];
        Net* net = nmap[nname];
        const vector<unsigned>& cells = net->cells();

        const unsigned toCount = net->count(!fromSide);

        unsigned jdx, cnt;
        switch (toCount) {
            case 0:
                --cutsize_reduction;
                for (jdx = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    store_updates<Mod::Inc>(cmap, records, other_name);
                }
                store_updates<Mod::Inc>(cmap, records, cname);
                assert(records[cname].updated == cell->gain());
                break;
            case 1:
                for (jdx = cnt = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    Cell* other_end = cmap[cells[jdx]];
                    if (other_end->side() != fromSide) {
                        store_updates<Mod::Dec>(cmap, records, other_name);
                        ++cnt;
                    }
                }
                assert(cnt == 1);
                break;
        }

        // cell is moved here
        cell->flip();
        assert(cell->side() != fromSide && "Cell should be flipped here");

        net->dec_count(fromSide);
        const unsigned fromCount = net->count(fromSide);

        switch (fromCount) {
            case 0:
                ++cutsize_reduction;
                for (jdx = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    store_updates<Mod::Dec>(cmap, records, other_name);
                }
                store_updates<Mod::Dec>(cmap, records, cname);
                break;
            case 1:
                for (jdx = cnt = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    Cell* other_end = cmap[cells[jdx]];
                    if (other_end->side() == fromSide) {
                        store_updates<Mod::Inc>(cmap, records, other_name);
                        ++cnt;
                    }
                }
                assert(cnt == 1);
                break;
            default:
                // Do nothing.
                break;
        }

        cell->flip();
        assert(cell->side() == fromSide && "Cell shouldn't be flipped here");

        assert(fromCount + toCount + 1 == cells.size() && "Counting error!");
    }
    cell->flip();
    assert(cell->side() != fromSide && "Cell should be flipped here");

    return cutsize_reduction;
}

int FloorPlan::flip(Bucket& nbucket,
                    const unordered_set<unsigned>& seen,
                    unsigned cname) {
    unordered_map<unsigned, GainChange> records;

    if (_cmap[cname]->side()) {
        --_tcount;
    } else {
        ++_tcount;
    }
    int gain = flip_cell(_nmap, _cmap, cname, records);

    for (auto iter = records.begin(); iter != records.end(); ++iter) {
        const unsigned name = iter->first;
        const GainChange& record = iter->second;
        if (seen.contains(name)) {
            assert(!_bucket.contains(name));
            nbucket.update(record.original, record.updated, name);
        } else {
            assert(!nbucket.contains(name));
            _bucket.update(record.original, record.updated, name);
        }
    }

    return gain;
}
