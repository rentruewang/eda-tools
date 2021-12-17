#include "sa.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <algorithm>

#include "costs.h"

using namespace std;

static constexpr double epsilon = 1e-7;

unsigned total_area(const pair<int, int> dim) {
    assert(dim.first >= 0);
    assert(dim.second >= 0);
    return (unsigned)dim.first * (unsigned)dim.second;
}

unsigned total_hpwl(const vector<Pin>& pin_list, const vector<Net>& net_list) {
    unsigned idx, hpwl;
    for (idx = hpwl = 0; idx < net_list.size(); ++idx) {
        const Net& net = net_list[idx];
        hpwl += net.hpwl();
    }
    return hpwl;
}

double total_cost(const double area, const double hpwl, const double alpha) {
    return alpha * area + (1 - alpha) * hpwl;
}

static void backup(const vector<Pin>& pin_list,
                   vector<Pin>& best,
                   const unsigned size) {
    best = vector<Pin>(pin_list.begin(), pin_list.begin() + size);
}

static void restore(vector<Pin>& pin_list,
                    const vector<Pin>& best,
                    const unsigned size) {
    copy(best.begin(), best.end(), pin_list.begin());
}

static unsigned random_cdf(double a, double b, double c) {
    constexpr double rand_range = (double)RAND_MAX + 1.;

    double rand_double = (double)rand() / (double)rand_range;
    assert(b >= a);
    assert(c >= b);
    assert(rand_double >= 0. && rand_double < 1.);

    if (rand_double < a) {
        return 0;
    } else if (rand_double < b) {
        return 1;
    } else if (rand_double < c) {
        return 2;
    } else {
        return 3;
    }
}

static bool is_close(double a, double b) {
    return (a - b) < epsilon && (b - a) < epsilon;
}

pair<int, int> SA(pair<unsigned, unsigned> boundary,
                  BStar& tree,
                  vector<Pin>& pin_list,
                  const vector<Net>& net_list,
                  pair<unsigned, unsigned> iter_info,
                  unsigned num_blocks,
                  unsigned episodes,
                  unsigned high_temp,
                  double alpha,
                  double ratio,
                  double P,
                  double C) {
    const unsigned width = boundary.first, height = boundary.second;
    const unsigned epochs = iter_info.first, interrupt = iter_info.second;
    const unsigned size = tree.get_size();
    const unsigned net_upper_bound = (width + height) * net_list.size();

    auto initial = tree.update();
    tree.check();

    printf("initial = (%d, %d)\n", initial.first, initial.second);

    const unsigned init_width = initial.first, init_height = initial.second;
    const unsigned init_hpwl = total_hpwl(pin_list, net_list);
    const double init_temp = 1. / log(1. / P);

    Cost cost_function =
        Cost(init_width * init_height, init_hpwl, alpha, ratio);

    for (unsigned idx = 0; idx < size; ++idx) {
        assert(pin_list[idx].area_nonzero());
    }
    for (unsigned idx = size; idx < pin_list.size(); ++idx) {
        assert(!pin_list[idx].area_nonzero());
    }

    vector<Pin> best_solution, best_accepted_solution;

    double best_cost = cost_function(init_width, width, init_height, height,
                                     init_hpwl, net_upper_bound, false);
    int best_width = init_width, best_height = init_height;

    backup(pin_list, best_solution, size);

    bool has_accepted;
    int iteration;

    for (has_accepted = false, iteration = -(high_temp * num_blocks);
         iteration < (int)(epochs * num_blocks); ++iteration) {
        double T;
        if (iteration < 0) {
            T = init_temp;
        } else if (iteration < (int)interrupt) {
            T = num_blocks * init_temp / (iteration * C);
        } else {
            T = num_blocks * init_temp / iteration;
        }

        unsigned idx;
        unsigned area_accepted, width_accepted, height_accepted;
        unsigned score_rdi, score_rp, score_rs, score_rm;
        double rdi_rp, rp_rs, rs_rm;

        for (idx = area_accepted = width_accepted = height_accepted = 0,
            rdi_rp = 1. / 4., rp_rs = 2. / 4., rs_rm = 3. / 4.;
             idx < episodes; ++idx) {
            score_rdi = score_rp = score_rs = score_rm = 0;

            pair<pair<unsigned, bool>, pair<unsigned, bool>> rdi;
            pair<unsigned, unsigned> rs;
            unsigned rp = 0, rm = 0;

            const char number = random_cdf(rdi_rp, rp_rs, rs_rm);
            switch (number) {
                case 0:
                    rdi = tree.random_delete_insert();
                    break;
                case 1:
                    rp = tree.random_permute();
                    break;
                case 2:
                    rs = tree.random_swap();
                    break;
                default:
                    assert(number == 3);
                    rm = tree.random_mirror();
                    break;
            }

            const auto plan = tree.update();
            tree.check();

            const unsigned wire_len = total_hpwl(pin_list, net_list);

            const bool width_okay = (plan.first <= (int)width),
                       height_okay = (plan.second <= (int)height),
                       area_okay = (plan.first * plan.second <
                                    (int)width * (int)height);
            const bool either_okay =
                (width_okay && height_okay) ||
                (plan.first <= (int)height && plan.second <= (int)width);

            assert(plan.first >= 0);
            assert(plan.second >= 0);

            if (!has_accepted && either_okay) {
                assert(area_okay);

                printf(
                    "size: (%d, %d) type: (%d, %d)\n", plan.first, plan.second,
                    (width_okay && height_okay),
                    (plan.first <= (int)height && plan.second <= (int)width));

                best_cost = INFINITY;
                has_accepted = true;
                best_width = best_height = -1;
            }

            const double cost =
                cost_function(plan.first, width, plan.second, height, wire_len,
                              net_upper_bound, has_accepted);

            bool acpt;

            if (!has_accepted || either_okay) {
                if (cost <= best_cost) {
                    best_cost = cost;
                    best_width = plan.first;
                    best_height = plan.second;
                    backup(pin_list, best_solution, size);
                    acpt = true;
                } else {
                    acpt = ((double)rand() / (double)RAND_MAX) <
                           expf((best_cost - cost) / T);
                }
            } else {
                acpt = false;
            }

            if (acpt) {
                switch (number) {
                    case 0:
                        ++score_rdi;
                        break;
                    case 1:
                        ++score_rp;
                        break;
                    case 2:
                        ++score_rs;
                        break;
                    default:
                        assert(number == 3);
                        ++score_rm;
                        break;
                }
            } else {
                switch (number) {
                    case 0:
                        tree.revert_delete_insert(rdi);
                        break;
                    case 1:
                        tree.revert_permute(rp);
                        break;
                    case 2:
                        tree.revert_swap(rs);
                        break;
                    default:
                        assert(number == 3);
                        tree.revert_mirror(rm);
                        break;
                }
            }

            if (width_okay)
                ++width_accepted;
            if (height_okay)
                ++height_accepted;
            if (area_okay)
                ++area_accepted;

            cost_function.update_width(!width_okay, episodes);
            cost_function.update_height(!height_okay, episodes);
        }

        // cost_function.update_alpha(!area_accepted, epochs);
        // cost_function.update_width(!width_accepted, epochs);
        // cost_function.update_height(!height_accepted, epochs);

        double total_score = score_rdi + score_rp + score_rs + score_rm;
        rdi_rp = (double)score_rdi / total_score;
        rp_rs = rdi_rp + (double)score_rp / total_score;
        rs_rm = rp_rs + (double)score_rs / total_score;
    }
    printf("a=%lf, w=%lf, h=%lf\n", cost_function.get_alpha(),
           cost_function.get_width(), cost_function.get_height());

    printf("solution found: %d\n", has_accepted);
    restore(pin_list, best_solution, size);

    if ((best_width > best_height) != (width > height)) {
        tree.flip();
        swap(best_width, best_height);
    }

    return make_pair(best_width, best_height);
}
