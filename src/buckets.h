#ifndef BUCKETS_H
#define BUCKETS_H

#include <map>
#include <memory>
#include <unordered_set>

#include "assertions.h"
#include "cells.h"

class List {
    std::unordered_set<unsigned> cells;

   public:
    List() : cells(std::unordered_set<unsigned>()) {}

    const std::unordered_set<unsigned> &getList() const { return cells; }

    unsigned size() const { return cells.size(); }
    void push(unsigned name) {
        non_release_mode();
        assert(cells.find(name) == cells.end());
        cells.insert(name);
    }

    unsigned pop() {
        auto last(cells.begin());
        non_release_mode();
        assert(cells.size() != 0);
        const unsigned cell = *last;
        cells.erase(last);
        return cell;
    }

    bool contains(const unsigned name) const {
        return cells.find(name) != cells.end();
    }
    void erase(const unsigned name) { cells.erase(name); }
};

class Bucket {
    std::map<int, List> bucket;

   public:
    Bucket() : bucket(std::map<int, List>()) {}
    Bucket(const std::vector<Cell *> &cell_map)
        : bucket(std::map<int, List>()) {
        fill(cell_map);
    }

    const std::map<int, List> &getBucket() const { return bucket; }

    void push(const unsigned name, const Cell *cell) {
        int gain(cell->getGain());
        bucket[gain].push(name);
    }
    unsigned pop() {
        auto max_iter(bucket.rbegin());
        List &list = max_iter->second;

        non_release_mode();
        assert(list.size() != 0);

        const unsigned cell(list.pop());
        if (list.size() == 0) bucket.erase(max_iter->first);

        return cell;
    }
    unsigned size() const {
        unsigned count(0);
        for (auto iter(bucket.begin()); iter != bucket.end(); ++iter) {
            count += iter->second.size();
        }
        return count;
    }

    bool contains(unsigned name) {
        for (auto iter(bucket.begin()); iter != bucket.end(); ++iter) {
            if (iter->second.contains(name)) return true;
        }
        return false;
    }

    void update(int old_gain, int new_gain, unsigned name) {
        non_release_mode();
        auto &old_list = bucket[old_gain];
        assert(old_list.contains(name));
        old_list.erase(name);
        if (old_list.size() == 0) {
            bucket.erase(old_gain);
        }
        auto &new_list = bucket[new_gain];
        assert(!new_list.contains(name));
        new_list.push(name);
    }

    void fill(const std::vector<Cell *> &cell_map) {
        for (unsigned idx(0); idx < cell_map.size(); ++idx) {
            push(idx, cell_map[idx]);
        }

#ifdef NDEBUG
        return;
#endif
        non_release_mode();
        assert(size() == cell_map.size());
    }

    Bucket &operator=(Bucket &&b) {
        bucket = std::move(b.bucket);
        return *this;
    }

    void empty(Bucket &other, std::unordered_set<unsigned> &seen) {
        for (auto iter(other.bucket.begin()); iter != other.bucket.end();
             ++iter) {
            const unsigned gain(iter->first);
            List &list = iter->second;
            assert(list.size() != 0);
            const unsigned SIZE(list.size());
            for (unsigned idx(0); idx < SIZE; ++idx) {
                unsigned value(list.pop());
                bucket[gain].push(value);
                seen.insert(value);
            }
        }
    }
};

#endif