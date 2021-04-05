#include "fm.h"
#include "helper.h"
#include "io.h"

#include <iostream>
#include <utility>

using namespace std;

int fm_once(vector<Net*>& nmap,
            vector<Cell*>& cmap,
            Bucket& bucket,
            function<bool(const unsigned)> condition,
            unsigned& partition) {
    unordered_set<unsigned> seen;
    Bucket nbucket;
    vector<unsigned> history;

    const unsigned SIZE = cmap.size();
    assert(bucket.size() == SIZE);
    assert(nbucket.size() == 0);

    int max_once = 1;
    unsigned max_idx, count;
    int max_gain, acc, once;
    for (count = max_idx = 0, max_gain = acc = 0; count < SIZE; ++count) {
        const unsigned current_cname = bucket.pop();
        Cell* cell = cmap[current_cname];

        assert(!seen.contains(current_cname));
        seen.insert(current_cname);
        nbucket.push(current_cname, cell);

        const unsigned npartition =
            cell->getSide() ? partition - 1 : partition + 1;

        if (!condition(npartition)) {
            continue;
        }

        partition = npartition;

        once = flip(nmap, cmap, bucket, nbucket, seen, current_cname, cell);
        acc += once;

        if (max_once < once) {
            max_once = once;
        }

        assert(seen.contains(current_cname));
        history.push_back(current_cname);
        if (acc >= max_gain) {
            max_gain = acc;
            max_idx = history.size();
        }
        if (max_once == 1 && once < 0) {
            nbucket.empty(bucket, seen);
            break;
        }
    }

    int total = acc;

    for (count = max_idx; count < history.size(); ++count) {
        const unsigned curr_cname = history[count];
        Cell* cell = cmap[curr_cname];

        partition = cell->getSide() ? partition - 1 : partition + 1;

        assert(seen.contains(curr_cname));
        once = flip(nmap, cmap, bucket, nbucket, seen, curr_cname, cell);

        total += once;
    }

    assert(bucket.size() == 0);
    assert(nbucket.size() == SIZE);

    bucket = move(nbucket);

    assert(max_gain == total && "Rewind is incorrect");

    for (auto iter = bucket.get().begin(); iter != bucket.get().end(); ++iter) {
        assert(iter->second.size() != 0);
    }

    silent_printf("Made %u moves\n", max_idx);
    return max_gain;
}

void fm(vector<Net*>& nmap,
        vector<Cell*>& cmap,
        const unsigned tolerate,
        unsigned& part) {
    const unsigned SIZE = cmap.size();
    const unsigned MIDDLE = SIZE >> 1;
    const unsigned LOWER = MIDDLE - tolerate, UPPER = MIDDLE + tolerate;
    Bucket bucket(cmap);

    silent_printf("between (%d, %d)\n", LOWER, UPPER);

    auto runFunc = [&](unsigned iter) {
        auto balanced = [=](const unsigned size) {
            return size > LOWER && size < UPPER;
        };
        int result = fm_once(nmap, cmap, bucket, balanced, part);
        silent_printf("On iteration = %03d, gain = %06d\n", iter, result);
        return result > 0;
    };

    for (unsigned iter = 0; runFunc(iter); ++iter)
        ;

    unsigned index = 0, count_true = 0;
    for (index = count_true = 0; index < cmap.size(); ++index) {
        count_true += cmap[index]->getSide();
    }
    assert(count_true == part);
}
