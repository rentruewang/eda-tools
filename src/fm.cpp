#include "fm.h"

#include <iostream>
#include <utility>

using namespace std;

bool meta_data(unsigned num_iter, int result) {
#ifndef SILENT
    printf("On iteration = %03d, gain = %06d\n", num_iter, result);
#endif
    return result > 0;
}

constexpr bool INC = true;
constexpr bool DEC = false;

void modify_gain(vector<Net *> &net_map, vector<Cell *> &cell_map,
                 unordered_map<unsigned, pair<int, int>> &records,
                 const unsigned name, const bool increase) {
    Cell *cell = cell_map[name];
    int old_gain = cell->getGain();
    int new_gain;
    if (increase == INC) {
        new_gain = old_gain + 1;
        cell->incGain();
    } else {
        assert(increase == DEC);
        new_gain = old_gain - 1;
        cell->decGain();
    }
    assert(new_gain == cell->getGain());
    if (records.find(name) == records.end()) {
        records[name] = pair<int, int>(old_gain, new_gain);
    } else {
        pair<int, int> &record = records[name];
        assert(old_gain == record.second);
        record.second = new_gain;
    }
}

int flip_cell(vector<Net *> &net_map, vector<Cell *> &cell_map,
              const unsigned cell_name, Cell *cell,
              unordered_map<unsigned, pair<int, int>> &records) {
    assert(records.size() == 0);

    int cutsize_reduction = 0;

    const vector<unsigned> &nets = cell->getNets();
    const bool fromSide = cell->getSide();

    for (unsigned idx = 0; idx < nets.size(); ++idx) {
        const unsigned net_name = nets[idx];
        Net *net = net_map[net_name];
        const vector<unsigned> &cells = net->getCells();

        const unsigned toCount = net->countOn(!fromSide);

        unsigned jdx, cnt;
        switch (toCount) {
            case 0:
                --cutsize_reduction;
                for (jdx = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    modify_gain(net_map, cell_map, records, other_name, INC);
                }
                modify_gain(net_map, cell_map, records, cell_name, INC);
                assert(records[cell_name].second == cell->getGain());
                break;
            case 1:
                for (jdx = cnt = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    Cell *other_end = cell_map[cells[jdx]];
                    if (other_end->getSide() != fromSide) {
                        modify_gain(net_map, cell_map, records, other_name,
                                    DEC);
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
                    modify_gain(net_map, cell_map, records, other_name, DEC);
                }
                modify_gain(net_map, cell_map, records, cell_name, DEC);
                break;
            case 1:
                for (jdx = cnt = 0; jdx < cells.size(); ++jdx) {
                    const unsigned other_name = cells[jdx];
                    Cell *other_end = cell_map[cells[jdx]];
                    if (other_end->getSide() == fromSide) {
                        modify_gain(net_map, cell_map, records, other_name,
                                    INC);
                        ++cnt;
                    }
                }
                assert(cnt == 1);
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

int wrap_flip_cell(vector<Net *> &net_map, vector<Cell *> &cell_map,
                   Bucket &bucket, Bucket &next_bucket,
                   const unordered_set<unsigned> &seen,
                   const unsigned cell_name, Cell *cell) {
    unordered_map<unsigned, pair<int, int>> records;
    int gain = flip_cell(net_map, cell_map, cell_name, cell, records);

    for (auto iter = records.begin(); iter != records.end(); ++iter) {
        const unsigned name = iter->first;
        const pair<int, int> &record = iter->second;
        if (seen.find(name) == seen.end()) {
            assert(!next_bucket.contains(name));
            bucket.update(record.first, record.second, name);
        } else {
            assert(!bucket.contains(name));
            next_bucket.update(record.first, record.second, name);
        }
    }

    return gain;
}

int fm_once(vector<Net *> &net_map, vector<Cell *> &cell_map, Bucket &bucket,
            function<bool(const unsigned)> condition, unsigned &partition) {
    unordered_set<unsigned> seen;
    Bucket next_bucket;
    vector<unsigned> history;
    unsigned max_idx, count;
    int max_gain, acc_gain, single_gain, max_single_gain = 1;

    const unsigned SIZE = cell_map.size();
    assert(bucket.size() == SIZE);
    assert(next_bucket.size() == 0);

    for (count = max_idx = 0, max_gain = acc_gain = 0; count < SIZE; ++count) {
        const unsigned current_cell_name = bucket.pop();
        Cell *cell = cell_map[current_cell_name];

        assert(seen.find(current_cell_name) == seen.end());
        seen.insert(current_cell_name);
        next_bucket.push(current_cell_name, cell);

        const unsigned next_partition =
            cell->getSide() ? partition - 1 : partition + 1;

        if (condition(next_partition)) {
            partition = next_partition;

            single_gain = wrap_flip_cell(net_map, cell_map, bucket, next_bucket,
                                         seen, current_cell_name, cell);
            acc_gain += single_gain;

            if (max_single_gain < single_gain) max_single_gain = single_gain;

            assert(seen.find(current_cell_name) != seen.end());
            history.push_back(current_cell_name);
            if (acc_gain >= max_gain) {
                max_gain = acc_gain;
                max_idx = history.size();
            }
            if (max_single_gain == 1 && single_gain < 0) {
                next_bucket.empty(bucket, seen);
                break;
            }
        }
    }

    int total_gain = acc_gain;

    for (count = max_idx; count < history.size(); ++count) {
        const unsigned current_cell_name = history[count];
        Cell *cell = cell_map[current_cell_name];

        partition = cell->getSide() ? partition - 1 : partition + 1;

        assert(seen.find(current_cell_name) != seen.end());
        single_gain = wrap_flip_cell(net_map, cell_map, bucket, next_bucket,
                                     seen, current_cell_name, cell);

        total_gain += single_gain;
    }

    assert(bucket.size() == 0);
    assert(next_bucket.size() == SIZE);

    bucket = move(next_bucket);

    assert(max_gain == total_gain && "Rewind is incorrect");

    for (auto iter = bucket.getBucket().begin();
         iter != bucket.getBucket().end(); ++iter) {
        assert(iter->second.size() != 0);
    }

#ifndef SILENT
    printf("Made %u moves\n", max_idx);
#endif
    return max_gain;
}

void fm(vector<Net *> &net_map, vector<Cell *> &cell_map,
        const unsigned tolerate, unsigned &part) {
    const unsigned SIZE = cell_map.size();
    const unsigned MIDDLE = SIZE >> 1;
    const unsigned LOWER = MIDDLE - tolerate, UPPER = MIDDLE + tolerate;
    Bucket bucket(cell_map);

#ifndef SILENT
    printf("between (%d, %d)\n", LOWER, UPPER);
#endif

    for (unsigned iter = 0; meta_data(iter, fm_once(
                                                net_map, cell_map, bucket,
                                                [=](const unsigned size) {
                                                    return size > LOWER &&
                                                           size < UPPER;
                                                },
                                                part));
         ++iter)
        ;

    unsigned index = 0, count_true = 0;
    for (index = count_true = 0; index < cell_map.size(); ++index) {
        count_true += cell_map[index]->getSide();
    }
    assert(count_true == part);
}
