#include "pins.h"

#include <assert.h>

using namespace std;

Pin::Pin() : x(0), y(0), w(0), h(0), l(-1), r(-1), name("") {}

Pin::Pin(const Pin& pin)
    : x(pin.x),
      y(pin.y),
      w(pin.w),
      h(pin.h),
      l(pin.l),
      r(pin.r),
      name(pin.name) {}

Pin::Pin(Pin&& pin)
    : x(pin.x),
      y(pin.y),
      w(pin.w),
      h(pin.h),
      l(pin.l),
      r(pin.r),
      name(move(pin.name)) {}

Pin::Pin(unsigned i, unsigned j, bool b, const string& n)
    : l(-1), r(-1), name(n) {
    if (b) {
        set_loc(0, 0);
        set_dim(i, j);
    } else {
        set_loc(i, j);
        set_dim(0, 0);
    }
}

Pin& Pin::operator=(const Pin& pin) {
    x = pin.x;
    y = pin.y;
    w = pin.w;
    h = pin.h;
    l = pin.l;
    r = pin.r;
    name = pin.name;
    return *this;
}

Pin& Pin::operator=(Pin&& pin) {
    x = pin.x;
    y = pin.y;
    w = pin.w;
    h = pin.h;
    l = pin.l;
    r = pin.r;
    name = move(pin.name);
    return *this;
}

int Pin::get_xpos() const {
    return x;
}

int Pin::get_ypos() const {
    return y;
}

int& Pin::get_xpos_mut() {
    return x;
}

int& Pin::get_ypos_mut() {
    return y;
}

void Pin::set_xpos(int x) {
    this->x = x;
}

void Pin::set_ypos(int y) {
    this->y = y;
}

pair<int, int> Pin::get_loc() const {
    return make_pair(get_xpos(), get_ypos());
}

void Pin::set_loc(int x, int y) {
    set_xpos(x);
    set_ypos(y);
}

unsigned Pin::get_width() const {
    return w;
}

unsigned Pin::get_height() const {
    return h;
}

unsigned& Pin::get_width_mut() {
    return w;
}

unsigned& Pin::get_height_mut() {
    return h;
}

void Pin::set_width(unsigned w) {
    this->w = w;
}

void Pin::set_height(unsigned h) {
    this->h = h;
}

pair<unsigned, unsigned> Pin::get_dim() const {
    return make_pair(get_width(), get_height());
}

void Pin::set_dim(unsigned w, unsigned h) {
    set_width(w);
    set_height(h);
}

const string& Pin::get_name() const {
    return name;
}

void Pin::set_name(string&& n) {
    name = move(n);
}

string& Pin::get_name_mut() {
    return name;
}

void Pin::rotate() {
    assert(area_nonzero());
    unsigned w = get_width(), h = get_height();
    set_dim(h, w);
}

int Pin::get_left() const {
    return l;
}

int Pin::get_right() const {
    return r;
}

int& Pin::get_left_mut() {
    return l;
}

int& Pin::get_right_mut() {
    return r;
}

bool Pin::has_left() const {
    return get_left() >= 0;
}

bool Pin::has_right() const {
    return get_right() >= 0;
}

void Pin::set_left(int l) {
    this->l = l;
}

void Pin::set_right(int r) {
    this->r = r;
}

void Pin::rm_left() {
    set_left(-1);
}

void Pin::rm_right() {
    set_right(-1);
}

bool Pin::area_nonzero() const {
    return get_width() && get_height();
}

bool Pin::leaf() const {
    return (l < 0) && (r < 0);
}

void Pin::filter_area_nonzero(vector<Pin>& pin_list,
                              vector<unsigned>& block_list) {
    assert(block_list.size() == 0);
    for (unsigned i = 0; i < pin_list.size(); ++i) {
        if (pin_list[i].area_nonzero()) {
            block_list.push_back(i);
        }
    }
}
