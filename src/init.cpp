#include "init.h"

#include <algorithm>
#include <set>
#include <utility>

using namespace std;

unsigned naive_init_side(vector<Cell*>& cmap) {
    const unsigned SIZE = cmap.size();
    const unsigned HALF = SIZE >> 1;
    for (unsigned idx = 0, cnt = 0; idx < cmap.size(); ++idx, ++cnt) {
        cmap[idx]->setSide(cnt < HALF);
    }
    return HALF;
}

unsigned sophisticated_init_side(vector<Net*>& nmap,
                                 vector<Cell*>& cmap,
                                 const unsigned too_much) {
    const unsigned net_size = nmap.size();
    set<unsigned> cell_inited;

    auto net_list = vector<Net*>(nmap.begin(), nmap.end());
    auto net_cmp = [](const Net* n1, const Net* n2) {
        return n1->size() < n2->size();
    };
    sort(net_list.begin(), net_list.end(), net_cmp);

    unsigned net_idx, count_true, count_false;
    for (net_idx = count_true = count_false = 0; net_idx < net_size;
         ++net_idx) {
        const vector<unsigned>& associated = net_list[net_idx]->getCells();

        unsigned cell_idx, confirmed_true, confirmed_false, unconfirmed;
        for (cell_idx = confirmed_true = confirmed_false = unconfirmed = 0;
             cell_idx < associated.size(); ++cell_idx) {
            const unsigned name = associated[cell_idx];
            if (cell_inited.contains(name)) {
                const Cell* cell = cmap[name];
                if (cell->getSide()) {
                    ++confirmed_true;
                } else {
                    ++confirmed_false;
                }
            } else {
                ++unconfirmed;
            }
        }

        assert(confirmed_true + confirmed_false + unconfirmed ==
                   associated.size() &&
               "Arithmetic error!");

        bool push_to_true = confirmed_true > confirmed_false;
        if (push_to_true && count_true + unconfirmed < too_much) {
            for (cell_idx = 0; cell_idx < associated.size(); ++cell_idx) {
                const unsigned name = associated[cell_idx];
                if (!cell_inited.contains(name)) {
                    cell_inited.insert(name);
                    Cell* cell = cmap[name];
                    cell->setSide(true);
                    ++count_true;
                }
            }
        } else if (!push_to_true && count_false + unconfirmed < too_much) {
            for (cell_idx = 0; cell_idx < associated.size(); ++cell_idx) {
                const unsigned name = associated[cell_idx];
                if (!cell_inited.contains(name)) {
                    cell_inited.insert(name);
                    Cell* cell = cmap[name];
                    cell->setSide(false);
                    ++count_false;
                }
            }
        } else {
            // count + unconfirmed >= too_much
            if (push_to_true) {
                assert(count_false + unconfirmed < too_much &&
                       "Need to rethink");
                for (cell_idx = 0; cell_idx < associated.size(); ++cell_idx) {
                    const unsigned name = associated[cell_idx];
                    if (!cell_inited.contains(name)) {
                        cell_inited.insert(name);
                        Cell* cell = cmap[name];
                        cell->setSide(false);
                        ++count_false;
                    }
                }
            } else {
                assert(count_true + unconfirmed < too_much &&
                       "Need to rethink");
                for (cell_idx = 0; cell_idx < associated.size(); ++cell_idx) {
                    const unsigned name = associated[cell_idx];
                    if (!cell_inited.contains(name)) {
                        cell_inited.insert(name);
                        Cell* cell = cmap[name];
                        cell->setSide(true);
                        ++count_true;
                    }
                }
            }
        }
    }

    // Assertions
    assert(count_true + count_false == cmap.size());
    assert(cell_inited.size() == cmap.size());
    unsigned ctrue = 0, cfalse = 0;
    for (unsigned idx = 0; idx < cmap.size(); ++idx) {
        assert(cell_inited.contains(idx));
        if (cmap[idx]->getSide()) {
            ++ctrue;
        } else {
            ++cfalse;
        }
    }
    assert(ctrue == count_true && cfalse == count_false);
    assert(ctrue < too_much && cfalse < too_much && "Logical error");
    return count_true;
}

unsigned init_side(vector<Net*>& nmap,
                   vector<Cell*>& cmap,
                   const unsigned too_much,
                   const bool sophisticated) {
    if (sophisticated) {
        return sophisticated_init_side(nmap, cmap, too_much);
    } else {
        return naive_init_side(cmap);
    }
}

template <bool checking>
void gains(vector<Net*>& nmap, vector<Cell*>& cmap) {
    if (checking) {
#ifdef NDEBUG
        return;
#endif
        printf("Checking Nets and Cells\n");
    }

    vector<int> simulation = vector<int>(cmap.size(), 0);

    for (unsigned net_idx = 0; net_idx < nmap.size(); ++net_idx) {
        Net* net = nmap[net_idx];
        const vector<unsigned>& cells = net->getCells();
        unsigned idx, cnt;
        for (idx = cnt = 0; idx < cells.size(); ++idx) {
            const unsigned cname = cells[idx];
            Cell* cell = cmap[cname];
            if (cell->getSide()) {
                ++cnt;
            }
        }

        if (checking) {
            assert(net->trueCount() == cnt && "Net count error");
        } else {
            net->setCount(cnt);
        }

        if (cnt == 0 || cnt == cells.size()) {
            for (idx = 0; idx < cells.size(); ++idx) {
                const unsigned cname = cells[idx];
                --(simulation[cname]);
            }
        } else {
            unsigned count;
            if (cnt == 1) {
                for (idx = count = 0; idx < cells.size(); ++idx) {
                    const unsigned cname = cells[idx];
                    Cell* cell = cmap[cname];
                    if (cell->getSide()) {
                        ++(simulation[cname]);
                        ++count;
                    }
                }
                assert(count == 1 && "Update too much");
            }
            if (cnt + 1 == cells.size()) {
                for (idx = count = 0; idx < cells.size(); ++idx) {
                    const unsigned cname = cells[idx];
                    Cell* cell = cmap[cname];
                    if (!(cell->getSide())) {
                        ++(simulation[cname]);
                        ++count;
                    }
                }
                assert(count == 1 && "Update too much");
            }
        }
    }

    assert(simulation.size() == cmap.size());

    for (unsigned idx = 0; idx < simulation.size(); ++idx) {
        if (checking) {
            assert(cmap[idx]->getGain() == simulation[idx] &&
                   "Gain miscalculated");
        } else {
            cmap[idx]->setGain(simulation[idx]);
        }
    }
}

void init_gains(vector<Net*>& nmap, vector<Cell*>& cmap) {
    gains<false>(nmap, cmap);
}

void assert_gains(vector<Net*>& nmap, vector<Cell*>& cmap) {
    gains<true>(nmap, cmap);
}
