#pragma once

#include <vector>

#include "pins.h"

// Read only Net
class Net {
    std::vector<unsigned> connected_pins;
    const std::vector<Pin>* all_pins;

   public:
    Net();
    Net(Net&& net);
    Net(std::vector<unsigned>&& conn, const std::vector<Pin>& all_pins);

    Net& operator=(Net&& net);

    unsigned at(unsigned index) const;

    unsigned hpwl(void) const;
};
