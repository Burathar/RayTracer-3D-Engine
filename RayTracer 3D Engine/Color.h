#ifndef _COLOR_H
#define _COLOR_H

class color {
	double red_, green_, blue_, special_;

public:

	color();

	color(double, double, double, double);

	double get_color_red() const { return red_; }
	double get_color_green() const { return green_; }
	double get_color_blue() const { return blue_; }
	double get_color_special() const { return special_; }

	void set_color_red(double red_value) { red_ = red_value; }
	void set_color_green(double green_value) { green_ = green_value; }
	void set_color_blue(double blue_value) { blue_ = blue_value; }
	void set_color_special(double special_value) { special_ = special_value; }

	double brightness() const
	{
		return (red_ + green_ + blue_) / 3;
	}

	color color_scalar(double scalar) const
	{
		return color(red_ * scalar, green_ * scalar, blue_ * scalar, special_);
	}

	color color_add(color add_color) const
	{
		return color(red_ + add_color.get_color_red(), green_ + add_color.get_color_green(), blue_ + add_color.get_color_blue(), special_);
	}

	color color_multiply(color add_color) const
	{
		return color(red_ * add_color.get_color_red(), green_ * add_color.get_color_green(), blue_ * add_color.get_color_blue(), special_);
	}

	color color_average(color add_color) const
	{
		return color((red_ + add_color.get_color_red()) / 2, (green_ + add_color.get_color_green()) / 2, (blue_ + add_color.get_color_blue()) / 2, special_);
	}

	color clip() {
		const double alllight = red_ + green_ + blue_;
		const double excess_light = alllight - 3;
		if (excess_light > 0) {
			red_ = red_ + excess_light * (red_ / alllight);
			green_ = green_ + excess_light * (green_ / alllight);
			blue_ = blue_ + excess_light * (blue_ / alllight);
		}
		if (red_ > 1) { red_ = 1; }
		if (green_ > 1) { green_ = 1; }
		if (blue_ > 1) { blue_ = 1; }
		if (red_ < 0) { red_ = 0; }
		if (green_ < 0) { green_ = 0; }
		if (blue_ < 0) { blue_ = 0; }

		return color(red_, green_, blue_, special_);
	}
};

inline color::color() {
	red_ = 0.5;
	green_ = 0.5;
	blue_ = 0.5;
	special_ = 0;
}

inline color::color(double r, double g, double b, double s) {
	red_ = r;
	green_ = g;
	blue_ = b;
	special_ = s;
}

#endif
