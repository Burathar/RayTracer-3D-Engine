#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "Object.h"
#include "MyVector.h"
#include "Color.h"

class sphere : public object {
	my_vector center_;
	double radius_;
	color color_;

public:

	sphere();

	sphere(my_vector, double, color);

	my_vector get_sphere_center() const { return center_; }
	double get_sphere_radius() const { return radius_; }
	color get_color() override { return color_; }

	my_vector get_normal_at(my_vector point) override
	{
		return point.vect_add(center_.negative()).normalize();
	}

	double find_intersection(ray ray) override
	{
		const double b =
			2 * (ray.get_ray_origin().get_vect_x() - center_.get_vect_x()) * ray.get_ray_direction().get_vect_x() +
			2 * (ray.get_ray_origin().get_vect_y() - center_.get_vect_y()) * ray.get_ray_direction().get_vect_y() +
			2 * (ray.get_ray_origin().get_vect_z() - center_.get_vect_z()) * ray.get_ray_direction().get_vect_z();
		const double c =
			(ray.get_ray_origin().get_vect_x() - center_.get_vect_x()) * (ray.get_ray_origin().get_vect_x() - center_.get_vect_x()) +
			(ray.get_ray_origin().get_vect_y() - center_.get_vect_y()) * (ray.get_ray_origin().get_vect_y() - center_.get_vect_y()) +
			(ray.get_ray_origin().get_vect_z() - center_.get_vect_z()) * (ray.get_ray_origin().get_vect_z() - center_.get_vect_z())
			- (radius_ * radius_);

		return determine_intersection(b, c);
	}

private:
	double determine_intersection(double b, double c) const
	{
		const double discriminant = b * b - 4 * c;

		if (discriminant > 0) {
			//the ray intersects the sphere (at 2 positions)
			const double root_1 = ((-1 * b - sqrt(discriminant)) / 2) - 0.000001;

			if (root_1 > 0) {
				// the first route is the smallest positive root_
				return root_1;
			}
			// the second root is the smallest root
			const double root_2 = ((sqrt(discriminant) - b) / 2) - 0.000001;
			return root_2;
		}
		// the ray missed the Sphere
		return -1;
	}
};

inline sphere::sphere() {
	center_ = my_vector(0, 0, 0);
	radius_ = 1.0;
	color_ = color(.5, .5, .5, 0);
}

inline sphere::sphere(my_vector center_value, double radius_value, color color_value) {
	center_ = center_value;
	radius_ = radius_value;
	color_ = color_value;
}

#endif
