#include "helper.h"

#include <unordered_map>
#include <vector>

#include "assertions.h"
#include "cells.h"
#include "nets.h"

using namespace std;

template <Mod mod>
void store_updates(vector<Cell*>& cmap,
                   unordered_map<unsigned, GainChange>& records,
                   const unsigned name) {
    Cell* cell = cmap[name];
    int old_gain = cell->getGain();
    int new_gain;
    switch (mod) {
        case Mod::Inc:
            new_gain = old_gain + 1;
            cell->incGain();
            break;
        case Mod::Dec:
            new_gain = old_gain - 1;
            cell->decGain();
            break;
    }
    assert(new_gain == cell->getGain());
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
              const unsigned cname,
              Cell* cell,
              unordered_map<unsigned, GainChange>& records) {
    assert(records.size() == 0);

    int cutsize_reduction = 0;

    const vector<unsigned>& nets = cell->getNets();
    const bool fromSide = cell->getSide();

    for (unsigned idx = 0; idx < nets.size(); ++idx) {
        const unsigned nname = nets[idx];
        Net* net = nmap[nname];
        const vector<unsigned>& cells = net->getCells();

        const unsigned toCount = net->countOn(!fromSide);

        unsigned jdx, cnt;
        switch (toCount) {
            case 0:
                --cutsize_reduction;
                for (jdx = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    store_updates<Mod::Inc>(cmap, records, other_name);
                }
                store_updates<Mod::Inc>(cmap, records, cname);
                assert(records[cname].updated == cell->getGain());
                break;
            case 1:
                for (jdx = cnt = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    Cell* other_end = cmap[cells[jdx]];
                    if (other_end->getSide() != fromSide) {
                        store_updates<Mod::Dec>(cmap, records, other_name);
                        ++cnt;
                    }
                }
                assert(cnt == 1);
                break;
        }

        // cell is moved here
        cell->flip();
        assert(cell->getSide() != fromSide && "Cell should be flipped here");

        net->decCountOn(fromSide);
        const unsigned fromCount = net->countOn(fromSide);

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
                    if (other_end->getSide() == fromSide) {
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
        assert(cell->getSide() == fromSide && "Cell shouldn't be flipped here");

        assert(fromCount + toCount + 1 == cells.size() && "Counting error!");
    }
    cell->flip();
    assert(cell->getSide() != fromSide && "Cell should be flipped here");

    return cutsize_reduction;
}

int flip(vector<Net*>& nmap,
         vector<Cell*>& cmap,
         Bucket& bucket,
         Bucket& nbucket,
         const unordered_set<unsigned>& seen,
         const unsigned cname,
         Cell* cell) {
    unordered_map<unsigned, GainChange> records;
    int gain = flip_cell(nmap, cmap, cname, cell, records);

    for (auto iter = records.begin(); iter != records.end(); ++iter) {
        const unsigned name = iter->first;
        const GainChange& record = iter->second;
        if (seen.contains(name)) {
            assert(!bucket.contains(name));
            nbucket.update(record.original, record.updated, name);
        } else {
            assert(!nbucket.contains(name));
            bucket.update(record.original, record.updated, name);
        }
    }

    return gain;
}
