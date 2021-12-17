#pragma once

#include <string>
#include <utility>
#include <vector>

class Pin {
    int x, y;
    unsigned w, h;
    int l, r;
    std::string name;

   public:
    Pin();
    Pin(const Pin& pin);
    Pin(Pin&& pin);
    Pin(unsigned i, unsigned j, bool b, const std::string& n);

    Pin& operator=(const Pin& pin);
    Pin& operator=(Pin&& pin);

    int get_xpos() const;
    int get_ypos() const;

    int& get_xpos_mut();
    int& get_ypos_mut();

    void set_xpos(int x);
    void set_ypos(int y);

    std::pair<int, int> get_loc() const;
    void set_loc(int x, int y);

    unsigned get_width() const;
    unsigned get_height() const;

    unsigned& get_width_mut();
    unsigned& get_height_mut();

    void set_width(unsigned w);
    void set_height(unsigned h);

    std::pair<unsigned, unsigned> get_dim() const;
    void set_dim(unsigned w, unsigned h);

    const std::string& get_name() const;
    void set_name(std::string&& n);

    std::string& get_name_mut();

    void rotate();

    int get_left() const;
    int get_right() const;

    int& get_left_mut();
    int& get_right_mut();

    bool has_left() const;
    bool has_right() const;

    void set_left(int l);
    void set_right(int r);

    void rm_left();
    void rm_right();

    bool area_nonzero() const;
    bool leaf() const;

    static void filter_area_nonzero(std::vector<Pin>& pin_list,
                                    std::vector<unsigned>& block_list);
};
