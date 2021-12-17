#include "costs.h"

#include <assert.h>
#include <math.h>

#include <algorithm>

using namespace std;

Cost::Cost() : area(1.), hpwl(1.), alpha(.5) {}

Cost::Cost(double init_area,
           double init_hpwl,
           double init_alpha,
           double aspect_ratio)
    : area(init_area),
      hpwl(init_hpwl),
      alpha(init_alpha),
      width(aspect_ratio / 2.),
      height(aspect_ratio / 2.) {
    assert(init_area >= 0.);
    assert(init_hpwl >= 0.);
    assert(init_alpha >= 0. && init_alpha <= 1.);
    assert(aspect_ratio >= 0. && aspect_ratio <= 1.);
}

double Cost::get_area() const {
    return area;
}

double Cost::get_hpwl() const {
    return hpwl;
}

double Cost::get_alpha() const {
    return alpha;
}

double Cost::get_width() const {
    return width;
}

double Cost::get_height() const {
    return height;
}

void Cost::set_area(double a) {
    area = a;
}

void Cost::set_hpwl(double h) {
    hpwl = h;
}

void Cost::set_alpha(double a) {
    alpha = a;
}

void Cost::set_width(double w) {
    width = w;
}

void Cost::set_height(double h) {
    height = h;
}

double& Cost::get_alpha_mut() {
    return alpha;
}

double& Cost::get_width_mut() {
    return width;
}

double& Cost::get_height_mut() {
    return height;
}

void Cost::rotate() {
    swap(width, height);
}

static double square(double v) {
    return v * v;
}

static void update_variable(double& reference, double target, unsigned rounds) {
    double coeff = 1. / (double)rounds;
    double remaining = 1. - coeff;

    assert(coeff > 0);
    assert(remaining >= 0 && remaining < 1);

    reference = remaining * reference + coeff * target;
}

void Cost::update_alpha(bool up, unsigned rounds) {
    update_variable(alpha, (double)up, rounds);
}

void Cost::update_width(bool up, unsigned rounds) {
    update_variable(width, ((double)up) / 2., rounds);
}

void Cost::update_height(bool up, unsigned rounds) {
    update_variable(height, ((double)up) / 2., rounds);
}

double Cost::operator()(unsigned X,
                        unsigned XN,
                        unsigned Y,
                        unsigned YN,
                        unsigned W,
                        unsigned WN,
                        bool accepted) const {
    double value = (alpha * (double)(X * Y) / (double)(XN * YN) +
                    (1. - alpha) * ((double)W / (double)WN));

    if (!accepted) {
        value = (1. - width - height) * value +
                width * square((double)X / (double)XN) +
                height * square((double)Y / (double)YN);
    }

    return value;
}

double Cost::operator()(unsigned X,
                        unsigned XN,
                        unsigned Y,
                        unsigned YN,
                        unsigned W,
                        unsigned WN,
                        bool accepted,
                        unsigned num) {
    update_variable(area, X * Y, num);
    update_variable(hpwl, W, num);
    return operator()(X, XN, Y, YN, W, WN, accepted);
}
