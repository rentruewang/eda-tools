#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "buckets.h"
#include "cells.h"
#include "nets.h"

class FloorPlan {
   public:
    FloorPlan();
    FloorPlan(std::vector<Net*>& nmap, std::vector<Cell*>& cmap);
    FloorPlan(std::vector<Net*>&& nmap, std::vector<Cell*>&& cmap);
    ~FloorPlan();

    void fm();

    void nmap(std::vector<Net*>&& nmap);
    std::vector<Net*>& nmap();
    const std::vector<Net*>& nmap() const;

    void cmap(std::vector<Cell*>&& cmap);
    std::vector<Cell*>& cmap();
    const std::vector<Cell*>& cmap() const;

    void input(const std::string fname);
    void output(const std::string name);

    double balance() const;

    Bucket& bucket();
    const Bucket& bucket() const;

    template <bool soph>
    void init_side();

    void tolerate(unsigned amount);

   private:
    std::vector<Net*> _nmap;
    std::vector<Cell*> _cmap;
    std::vector<std::string> _nnames;
    std::vector<std::string> _cnames;
    Bucket _bucket;
    double _balance;
    unsigned _tcount;
    unsigned _tolerate;

    unsigned naiv_init_side();
    unsigned soph_init_side();

    void init_gains();
    void check_gains();
    void init_bucket();

    template <bool check>
    void cal_gains();

    int fm_once(std::function<bool(const unsigned)> condition);

    int flip(Bucket& nbucket,
             const std::unordered_set<unsigned>& seen,
             unsigned cname);
};

template <bool soph>
void FloorPlan::init_side() {
    if (soph) {
        _tcount = soph_init_side();
    } else {
        _tcount = naiv_init_side();
    }
}
