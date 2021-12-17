#pragma once

class Cost {
    double area, hpwl;

    double alpha, width, height;

   public:
    Cost();
    Cost(double init_area,
         double init_hpwl,
         double init_alpha,
         double init_ratio);

    double get_area() const;
    double get_hpwl() const;

    double get_alpha() const;
    double get_width() const;
    double get_height() const;

    void set_area(double area);
    void set_hpwl(double hpwl);

    void set_alpha(double alpha);
    void set_width(double width);
    void set_height(double height);

    double& get_alpha_mut();
    double& get_width_mut();
    double& get_height_mut();

    void rotate();

    void update_alpha(bool up, unsigned rounds);
    void update_width(bool up, unsigned rounds);
    void update_height(bool up, unsigned rounds);

    double operator()(unsigned width,
                      unsigned width_norm,
                      unsigned height,
                      unsigned height_norm,
                      unsigned hpwl,
                      unsigned net_norm,
                      bool accepted) const;

    double operator()(unsigned width,
                      unsigned width_norm,
                      unsigned height,
                      unsigned height_norm,
                      unsigned hpwl,
                      unsigned net_norm,
                      bool accepted,
                      unsigned rounds);
};
