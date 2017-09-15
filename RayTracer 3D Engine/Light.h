#ifndef _LIGHT_H
#define _LIGHT_H

#include "Source.h"
#include "MyVector.h"
#include "Color.h"

class light : public source {
	my_vector position_;
	color color_;

public:

	light();

	light(my_vector, color);

	my_vector get_light_position() { return position_; }
	color get_light_color() { return color_; }
};

inline light::light() {
	position_ = my_vector(0, 0, 0);
	color_ = color(1, 1, 1, 0);
}

inline light::light(my_vector p, color c) {
	position_ = p;
	color_ = c;
}

#endif
