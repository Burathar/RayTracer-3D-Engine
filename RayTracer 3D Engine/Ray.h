#ifndef _RAY_H
#define _RAY_H

#include "MyVector.h"

class ray {
	my_vector origin_, direction_;

public:

	ray();

	ray(my_vector, my_vector);

	my_vector get_ray_origin() { return origin_; }
	my_vector get_ray_direction() { return direction_; }
};

inline ray::ray() {
	origin_ = my_vector(0, 0, 0);
	direction_ = my_vector(1, 0, 0);
}

inline ray::ray(my_vector o, my_vector d) {
	origin_ = o;
	direction_ = d;
}

#endif
