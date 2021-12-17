#include "b-star.h"

#include <assert.h>
#include <stdlib.h>

#include <algorithm>
#include <deque>
#include <functional>
#include <memory>
#include <vector>

#include "boundaries.h"

using namespace std;

BStar::BStar() : nodes(vector<unsigned>()), pin_list_ptr(nullptr) {}

// static unsigned parent(unsigned index) { return ((index - 1) >> 1); }

static unsigned left_child(unsigned index) {
    return (index << 1) + 1;
}

static unsigned right_child(unsigned index) {
    return (index << 1) + 2;
}

BStar::BStar(vector<Pin>& pin_list, unsigned width, unsigned height) {
    auto index_list = vector<unsigned>();
    pin_list_ptr = &pin_list;

    Pin::filter_area_nonzero(pin_list, index_list);

    for (unsigned idx = 0; idx < index_list.size(); ++idx) {
        Pin& pin = pin_list[index_list[idx]];
        if (pin.get_height() > pin.get_width()) {
            pin.rotate();
        }
        assert(pin.get_width() >= pin.get_height());
    }

    sort(index_list.rbegin(), index_list.rend(),
         [&](unsigned a, unsigned b) -> bool {
             const auto &pin_a = pin_list[a], &pin_b = pin_list[b];
             if (pin_a.get_width() < pin_b.get_width()) {
                 return true;
             }
             if (pin_a.get_width() > pin_b.get_width()) {
                 return false;
             }
             return pin_a.get_height() < pin_b.get_height();
         });

    // * heap update
    for (unsigned index = 0; index < index_list.size(); ++index) {
        const unsigned l = left_child(index), r = right_child(index);
        unsigned self = index_list[index];
        Pin& pin = pin_list[self];
        assert(pin.area_nonzero());

        assert(pin.get_left() < 0);
        assert(pin.get_right() < 0);
        if (r == index_list.size()) {
            pin.set_left(index_list[l]);
        } else if (r < index_list.size()) {
            pin.set_left(index_list[l]);
            pin.set_right(index_list[r]);
        }
    }

    // * line update
    // for (unsigned idx = 0, total = 0, last = 0; idx < index_list.size() - 1;
    //      ++idx) {
    //     const unsigned node_index = index_list[idx],
    //                    next_node_index = index_list[idx + 1];
    //     Pin &node = pin_list[node_index];
    //     const Pin &next_node = pin_list[next_node_index];
    //     assert(node.get_left() < 0 && node.get_right() < 0);
    //     assert(next_node.get_left() < 0 && next_node.get_right() < 0);
    //     if (total <= height) {
    //         node.set_right(next_node_index);
    //     } else {
    //         assert(last <= idx);
    //         pin_list[index_list[last]].set_left(next_node_index);
    //         total = 0;
    //         last = idx + 1;
    //     }
    //     total += node.get_height();
    // }

    this->nodes = move(index_list);
}

unsigned BStar::get_size() const {
    return nodes.size();
}

unsigned BStar::get_root() const {
    return nodes[0];
}

vector<Pin>& BStar::pin_list(void) {
    return *pin_list_ptr;
}

const vector<Pin>& BStar::pin_list(void) const {
    return *pin_list_ptr;
}

static unsigned size_recursive(unsigned root, const vector<Pin>& pin_list) {
    unsigned ans = 0;
    int id;
    const Pin& root_node = pin_list[root];
    if ((id = root_node.get_left()) >= 0) {
        ans += size_recursive(id, pin_list);
    }
    if ((id = root_node.get_right()) >= 0) {
        ans += size_recursive(id, pin_list);
    }
    return ans + 1;
}

static void overlap_recursive(unsigned root, const vector<Pin>& pin_list) {
    const Pin& root_node = pin_list[root];
    int id;

    if ((id = root_node.get_left()) >= 0) {
        const Pin& left_node = pin_list[id];
        assert(root_node.get_xpos() + (int)root_node.get_width() ==
               left_node.get_xpos());
    }

    if ((id = root_node.get_right()) >= 0) {
        const Pin& right_node = pin_list[id];
        assert(root_node.get_xpos() == right_node.get_xpos());
        assert(root_node.get_ypos() + (int)root_node.get_height() <=
               right_node.get_ypos());
    }

    if (root_node.get_left() >= 0) {
        overlap_recursive(root_node.get_left(), pin_list);
    }

    if (root_node.get_right() >= 0) {
        overlap_recursive(root_node.get_right(), pin_list);
    }
}

void BStar::check(void) const {
#ifdef NDEBUG
    return;
#endif
    assert(size_recursive(get_root(), pin_list()) == get_size());
    overlap_recursive(get_root(), pin_list());
}

static void remove_overlap(Pin& pin,
                           deque<Boundary>& contour,
                           const unsigned index) {
    assert(index <= contour.size());
    assert(contour.size());

    if (pin.get_xpos() >= contour[contour.size() - 1].right()) {
        assert(index == contour.size());
        pin.set_ypos(0);
        return;
    }

    const int total_width = pin.get_xpos() + pin.get_width();

    unsigned until;
    int max;

    for (until = index, max = -1;
         until < contour.size() && contour[until].right() < total_width;
         ++until) {
        if (contour[until].top() > max) {
            max = contour[until].top();
        }
    }

    assert(until <= contour.size());

    if (until == contour.size()) {
        contour.erase(contour.begin() + index, contour.end());
    } else {
        assert(until < contour.size());
        assert(contour[until].right() >= total_width);

        if (contour[until].top() > max) {
            max = contour[until].top();
        }

        for (; until < contour.size() && contour[until].right() == total_width;
             ++until)
            ;

        contour.erase(contour.begin() + index, contour.begin() + until);
    }
    pin.set_ypos(max);
}

static void update_left(vector<Pin>& pin_list,
                        unsigned root,
                        unsigned left,
                        deque<Boundary>& contour,
                        unsigned affected) {
    const Pin& root_node = pin_list[root];
    Pin& left_node = pin_list[left];

    assert(affected < contour.size());

    left_node.set_xpos(root_node.get_xpos() + root_node.get_width());

    const unsigned index = affected + 1;

    remove_overlap(left_node, contour, index);

    contour.insert(contour.begin() + index, move(Boundary(left_node)));

    assert(index < contour.size());
}

static void update_right(vector<Pin>& pin_list,
                         unsigned root,
                         unsigned right,
                         deque<Boundary>& contour,
                         unsigned affected) {
    const Pin& root_node = pin_list[root];
    Pin& right_node = pin_list[right];

    assert(affected < contour.size());

    right_node.set_xpos(root_node.get_xpos());

    const unsigned index = affected;

    remove_overlap(right_node, contour, index);

    contour.insert(contour.begin() + index, move(Boundary(right_node)));
    assert(index < contour.size());
}

static void update_recursive(vector<Pin>& pin_list,
                             unsigned root,
                             deque<Boundary>& contour,
                             unsigned affected) {
    const Pin& root_node = pin_list[root];

    Boundary& bnd = contour[affected];
    assert(bnd == Boundary(root_node));

    int left = root_node.get_left(), right = root_node.get_right();

    if (left >= 0) {
        update_left(pin_list, root, left, contour, affected);
        update_recursive(pin_list, left, contour, affected + 1);
    }

    if (right >= 0) {
        update_right(pin_list, root, right, contour, affected);
        update_recursive(pin_list, right, contour, affected);
    }
}

pair<int, int> BStar::update(void) const {
    auto& pin_list = *pin_list_ptr;
    auto contour = deque<Boundary>();
    int root = get_root();
    Pin& root_node = pin_list[root];
    root_node.set_loc(0, 0);
    contour.push_back(move(Boundary(root_node)));
    update_recursive(pin_list, root, contour, 0);

    int X, Y, i;
    for (i = X = Y = 0; (unsigned)i < contour.size(); ++i) {
        const Boundary& bnd = contour[i];
        if (Y < bnd.top())
            Y = bnd.top();

        if (X < bnd.right())
            X = bnd.right();
    }
    return make_pair(X, Y);
}

static void flip_recursive(unsigned root, vector<Pin>& pin_list) {
    Pin& pin = pin_list[root];
    swap(pin.get_xpos_mut(), pin.get_ypos_mut());
    swap(pin.get_width_mut(), pin.get_height_mut());
    const int left = pin.get_left(), right = pin.get_right();
    if (left >= 0) {
        flip_recursive(left, pin_list);
    }
    if (right >= 0) {
        flip_recursive(right, pin_list);
    }
}

void BStar::flip(void) {
    flip_recursive(get_root(), pin_list());
}

void BStar::permute(const unsigned idx) {
    pin_list()[nodes[idx]].rotate();
}

void BStar::swap(const unsigned i, const unsigned j) {
    auto& plist = pin_list();
    Pin &pin_i = plist[nodes[i]], &pin_j = plist[nodes[j]];

    assert(i != j);
    assert(pin_i.get_name() != pin_j.get_name());

    std::swap(pin_i.get_width_mut(), pin_j.get_width_mut());
    std::swap(pin_i.get_height_mut(), pin_j.get_height_mut());
    std::swap(pin_i.get_name_mut(), pin_j.get_name_mut());
}

unsigned BStar::random_permute(void) {
    unsigned r = rand() % nodes.size();
    permute(r);
    return r;
}

void BStar::delete_insert(unsigned from,
                          unsigned to,
                          bool from_side,
                          bool to_side) {
    auto& plist = pin_list();
    Pin& from_pin = plist[from];
    Pin& to_pin = plist[to];

    int target;

    if (from_side) {
        assert(from_pin.has_right());
        target = from_pin.get_right();
        from_pin.rm_right();
        assert(!from_pin.has_right());
    } else {
        assert(from_pin.has_left());
        target = from_pin.get_left();
        from_pin.rm_left();
        assert(!from_pin.has_left());
    }

    assert(target >= 0);

    const Pin& target_pin = plist[target];

    assert(target_pin.leaf());

    if (to_side) {
        assert(!to_pin.has_right());
        to_pin.set_right(target);
        assert(to_pin.has_right());
    } else {
        assert(!to_pin.has_left());
        to_pin.set_left(target);
        assert(to_pin.has_left());
    }
}

void BStar::mirror(const unsigned i) {
    Pin& pin = pin_list()[i];
    std::swap(pin.get_left_mut(), pin.get_right_mut());
}

constexpr bool fix_root = false;

pair<unsigned, unsigned> BStar::random_swap(void) {
    unsigned a, b;
    if (fix_root) {
        a = (rand() % (nodes.size() - 1)) + 1;
        b = (rand() % (nodes.size() - 2)) + 1;
        assert(a != 0);
        assert(b != 0);
    } else {
        a = rand() % nodes.size();
        b = rand() % (nodes.size() - 1);
    }
    if (b >= a)
        ++b;
    assert(a != b);
    this->swap(a, b);
    return make_pair(a, b);
}

pair<pair<unsigned, bool>, pair<unsigned, bool>> BStar::random_delete_insert(
    void) {
    int root;
    bool side;

    auto& plist = pin_list();

    if (plist[get_root()].leaf()) {
        return make_pair(make_pair(0U, false), make_pair(0U, false));
    }

    do {
        root = rand() % get_size();
    } while (plist[root].leaf());

    while (true) {
        const Pin& pin = plist[root];
        int left = pin.get_left(), right = pin.get_right();

        assert(!pin.leaf());

        bool both_fine = (left >= 0) && (right >= 0);

        if (left < 0 || (both_fine && (rand() % 2))) {
            assert(right >= 0 && right < (int)nodes.size());
            const Pin& right_node = plist[right];

            if (right_node.leaf()) {
                side = true;
                break;
            }

            root = right;
        } else {
            if (right < 0) {
                assert(left >= 0);
            } else {
                assert(both_fine);
            }

            const Pin& left_node = plist[left];

            if (left_node.leaf()) {
                side = false;
                break;
            }

            root = left;
        }
    }

    assert(root >= 0);

    const int from = root;
    const bool from_side = side;

    int next;

    do {
        root = rand() % get_size();
    } while (plist[root].leaf());

    while (true) {
        const Pin& pin = plist[root];
        if (root == from) {
            if (from_side) {
                next = pin.get_left();
                if (next < 0) {
                    side = false;
                    break;
                }

            } else {
                next = pin.get_right();
                if (next < 0) {
                    side = true;
                    break;
                }
            }
        } else {
            if (rand() % 2) {
                next = pin.get_right();
                if (next < 0) {
                    side = true;
                    break;
                }
            } else {
                next = pin.get_left();
                if (next < 0) {
                    side = false;
                    break;
                }
            }
        }
        root = next;
    }

    assert(root >= 0);

    const int to = root;
    const bool to_side = side;

    this->delete_insert(from, to, from_side, to_side);

    return make_pair(make_pair(from, from_side), make_pair(to, to_side));
}

unsigned BStar::random_mirror(void) {
    int r = rand() % nodes.size();
    this->mirror(r);
    return r;
}

void BStar::revert_permute(unsigned action) {
    this->permute(action);
}

void BStar::revert_swap(pair<unsigned, unsigned> action) {
    this->swap(action.first, action.second);
}

void BStar::revert_delete_insert(
    const pair<pair<unsigned, bool>, pair<unsigned, bool>> action) {
    const auto from_pair = action.first, to_pair = action.second;
    const auto from = from_pair.first, to = to_pair.first;
    const auto from_side = from_pair.second, to_side = to_pair.second;
    this->delete_insert(to, from, to_side, from_side);
}

void BStar::revert_mirror(unsigned i) {
    this->mirror(i);
}
