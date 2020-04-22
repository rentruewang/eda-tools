#include "buckets.h"

List::List() : cells(std::unordered_set<unsigned>()) {}

const std::unordered_set<unsigned> &List::getList() const { return cells; }

unsigned List::size() const { return cells.size(); }

void List::push(unsigned name) {
    assert(cells.find(name) == cells.end());
    cells.insert(name);
}

unsigned List::pop() {
    assert(cells.size() != 0);

    auto last = cells.begin();
    const unsigned cell = *last;
    cells.erase(last);

    return cell;
}

bool List::contains(const unsigned name) const {
    return cells.find(name) != cells.end();
}

void List::erase(const unsigned name) { cells.erase(name); }

Bucket::Bucket() : bucket(std::map<int, List>()) {}

Bucket::Bucket(const std::vector<Cell *> &cell_map)
    : bucket(std::map<int, List>()) {
    fill(cell_map);
}

const std::map<int, List> &Bucket::getBucket() const { return bucket; }

void Bucket::push(const unsigned name, const Cell *cell) {
    int gain = cell->getGain();
    bucket[gain].push(name);
}

unsigned Bucket::pop() {
    auto max_iter = bucket.rbegin();
    List &list = max_iter->second;

    assert(list.size() != 0);

    const unsigned cell = list.pop();
    if (list.size() == 0) bucket.erase(max_iter->first);

    return cell;
}

unsigned Bucket::size() const {
    unsigned count = 0;
    for (auto iter = bucket.begin(); iter != bucket.end(); ++iter) {
        count += iter->second.size();
    }
    return count;
}

bool Bucket::contains(unsigned name) {
    for (auto iter = bucket.begin(); iter != bucket.end(); ++iter) {
        if (iter->second.contains(name)) return true;
    }
    return false;
}

void Bucket::update(int old_gain, int new_gain, unsigned name) {
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

void Bucket::fill(const std::vector<Cell *> &cell_map) {
    for (unsigned idx = 0; idx < cell_map.size(); ++idx) {
        push(idx, cell_map[idx]);
    }
    assert(size() == cell_map.size());
}

Bucket &Bucket::operator=(Bucket &&b) {
    bucket = std::move(b.bucket);
    return *this;
}

void Bucket::empty(Bucket &other, std::unordered_set<unsigned> &seen) {
    for (auto iter = other.bucket.begin(); iter != other.bucket.end(); ++iter) {
        const unsigned gain = iter->first;
        List &list = iter->second;

        assert(list.size() != 0);

        for (unsigned idx = 0, SIZE = list.size(); idx < SIZE; ++idx) {
            unsigned value = list.pop();
            bucket[gain].push(value);
            seen.insert(value);
        }
    }
}
