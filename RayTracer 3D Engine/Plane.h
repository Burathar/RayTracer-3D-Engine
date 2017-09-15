#ifndef _PLANE_H
#define _PLANE_H

#include "Object.h"
#include "MyVector.h"
#include "Color.h"

class plane : public object {
	my_vector normal_;
	double distance_;
	color color_;

public:

	plane();

	plane(my_vector, double, color);

	my_vector get_plane_normal() { return normal_; }
	double get_plane_distance() { return distance_; }
	color get_color() override { return color_; }

	my_vector get_normal_at(my_vector point) override
	{
		return normal_;
	}

	double find_intersection(ray ray) override
	{
		my_vector ray_direction = ray.get_ray_direction();

		double a = ray_direction.dot_product(normal_);

		if (a == 0) {
			//ray is parallel to  the plane
			return -1;
		}
		else {
			const double b = normal_.dot_product(ray.get_ray_origin().vect_add(normal_.vect_mult(distance_).negative()));
			return -1 * b / a;
		}
	}
};

inline plane::plane() {
	normal_ = my_vector(1, 0, 0);
	distance_ = 0.0;
	color_ = color(.5, .5, .5, 0);
}

inline plane::plane(my_vector normalValue, double distanceValue, color colorValue) {
	normal_ = normalValue;
	distance_ = distanceValue;
	color_ = colorValue;
}

#endif
