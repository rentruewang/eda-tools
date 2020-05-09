#include "init.h"

#include <algorithm>
#include <set>
#include <utility>

using namespace std;

unsigned naive_init_side(vector<Cell*>& cell_map) {
    const unsigned SIZE = cell_map.size();
    const unsigned HALF = SIZE >> 1;
    for (unsigned idx = 0, cnt = 0; idx < cell_map.size(); ++idx, ++cnt) {
        cell_map[idx]->setSide(cnt < HALF);
    }
    return HALF;
}

unsigned sophisticated_init_side(vector<Net*>& net_map,
                                 vector<Cell*>& cell_map,
                                 const unsigned too_much) {
    const unsigned net_size = net_map.size();
    set<unsigned> cell_inited;

    auto net_list = vector<Net*>(net_map.begin(), net_map.end());
    sort(net_list.begin(), net_list.end(),
         [](const Net* n1, const Net* n2) { return n1->size() < n2->size(); });

    unsigned net_idx, count_true, count_false;
    for (net_idx = count_true = count_false = 0; net_idx < net_size;
         ++net_idx) {
        const vector<unsigned>& associated = net_list[net_idx]->getCells();

        unsigned cell_idx, confirmed_true, confirmed_false, unconfirmed;
        for (cell_idx = confirmed_true = confirmed_false = unconfirmed = 0;
             cell_idx < associated.size(); ++cell_idx) {
            const unsigned name = associated[cell_idx];
            if (cell_inited.find(name) == cell_inited.end()) {
                ++unconfirmed;
            } else {
                const Cell* cell = cell_map[name];
                if (cell->getSide()) {
                    ++confirmed_true;
                } else {
                    ++confirmed_false;
                }
            }
        }

        assert(confirmed_true + confirmed_false + unconfirmed ==
                   associated.size() &&
               "Arithmetic error!");

        bool push_to_true = confirmed_true > confirmed_false;
        if (push_to_true && count_true + unconfirmed < too_much) {
            for (cell_idx = 0; cell_idx < associated.size(); ++cell_idx) {
                const unsigned name = associated[cell_idx];
                if (cell_inited.find(name) == cell_inited.end()) {
                    cell_inited.insert(name);
                    Cell* cell = cell_map[name];
                    cell->setSide(true);
                    ++count_true;
                }
            }
        } else if (!push_to_true && count_false + unconfirmed < too_much) {
            for (cell_idx = 0; cell_idx < associated.size(); ++cell_idx) {
                const unsigned name = associated[cell_idx];
                if (cell_inited.find(name) == cell_inited.end()) {
                    cell_inited.insert(name);
                    Cell* cell = cell_map[name];
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
                    if (cell_inited.find(name) == cell_inited.end()) {
                        cell_inited.insert(name);
                        Cell* cell = cell_map[name];
                        cell->setSide(false);
                        ++count_false;
                    }
                }
            } else {
                assert(count_true + unconfirmed < too_much &&
                       "Need to rethink");
                for (cell_idx = 0; cell_idx < associated.size(); ++cell_idx) {
                    const unsigned name = associated[cell_idx];
                    if (cell_inited.find(name) == cell_inited.end()) {
                        cell_inited.insert(name);
                        Cell* cell = cell_map[name];
                        cell->setSide(true);
                        ++count_true;
                    }
                }
            }
        }
    }

    // Assertions
    assert(count_true + count_false == cell_map.size());
    assert(cell_inited.size() == cell_map.size());
    unsigned ctrue = 0, cfalse = 0;
    for (unsigned idx = 0; idx < cell_map.size(); ++idx) {
        assert(cell_inited.find(idx) != cell_inited.end());
        if (cell_map[idx]->getSide()) {
            ++ctrue;
        } else {
            ++cfalse;
        }
    }
    assert(ctrue == count_true && cfalse == count_false);
    assert(ctrue < too_much && cfalse < too_much && "Logical error");
    return count_true;
}

unsigned init_side(vector<Net*>& net_map,
                   vector<Cell*>& cell_map,
                   const unsigned too_much,
                   const bool sophisticated) {
    if (sophisticated) {
        return sophisticated_init_side(net_map, cell_map, too_much);
    } else {
        return naive_init_side(cell_map);
    }
}

void gains(vector<Net*>& net_map,
           vector<Cell*>& cell_map,
           const bool checking) {
    if (checking) {
#ifdef NDEBUG
        return;
#endif
        printf("Checking Nets and Cells\n");
    }

    vector<int> simulation;
    simulation.reserve(cell_map.size());
    for (unsigned idx = 0; idx < cell_map.size(); ++idx) {
        simulation.push_back(0);
    }

    for (unsigned net_idx = 0; net_idx < net_map.size(); ++net_idx) {
        Net* net = net_map[net_idx];
        const vector<unsigned>& cells = net->getCells();
        unsigned idx, cnt;
        for (idx = cnt = 0; idx < cells.size(); ++idx) {
            const unsigned cell_name = cells[idx];
            Cell* cell = cell_map[cell_name];
            if (cell->getSide())
                ++cnt;
        }

        if (checking) {
            assert(net->trueCount() == cnt && "Net count error");
        } else {
            net->setCount(cnt);
        }

        if (cnt == 0 || cnt == cells.size()) {
            for (idx = 0; idx < cells.size(); ++idx) {
                const unsigned cell_name = cells[idx];
                --(simulation[cell_name]);
            }
        } else {
            unsigned count;
            if (cnt == 1) {
                for (idx = count = 0; idx < cells.size(); ++idx) {
                    const unsigned cell_name = cells[idx];
                    Cell* cell = cell_map[cell_name];
                    if (cell->getSide())
                        ++(simulation[cell_name]), ++count;
                }
                assert(count == 1 && "Update too much");
            }
            if (cnt + 1 == cells.size()) {
                for (idx = count = 0; idx < cells.size(); ++idx) {
                    const unsigned cell_name = cells[idx];
                    Cell* cell = cell_map[cell_name];
                    if (!(cell->getSide()))
                        ++(simulation[cell_name]), ++count;
                }
                assert(count == 1 && "Update too much");
            }
        }
    }

    assert(simulation.size() == cell_map.size());

    for (unsigned idx = 0; idx < simulation.size(); ++idx) {
        if (checking) {
            assert(cell_map[idx]->getGain() == simulation[idx] &&
                   "Gain miscalculated");
        } else {
            cell_map[idx]->setGain(simulation[idx]);
        }
    }
}

void init_gains(vector<Net*>& net_map, vector<Cell*>& cell_map) {
    gains(net_map, cell_map, false);
}

void assert_gains(vector<Net*>& net_map, vector<Cell*>& cell_map) {
    gains(net_map, cell_map, true);
}
