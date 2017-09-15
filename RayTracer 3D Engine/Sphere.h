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

	my_vector get_sphere_center() { return center_; }
	double get_sphere_radius() { return radius_; }
	color get_color() override { return color_; }

	my_vector get_normal_at(my_vector point) override
	{
		return point.vect_add(center_.negative()).normalize();
	}

	double find_intersection(ray ray) override
	{
		my_vector ray_origin = ray.get_ray_origin();
		double ray_origin_x = ray_origin.get_vect_x();
		double ray_origin_y = ray_origin.get_vect_y();
		double ray_origin_z = ray_origin.get_vect_z();

		my_vector ray_direction = ray.get_ray_direction();
		double ray_direction_x = ray_direction.get_vect_x();
		double ray_direction_y = ray_direction.get_vect_y();
		double ray_direction_z = ray_direction.get_vect_z();

		my_vector sphere_center = center_;//zwaar onnodig?
		double sphere_center_x = sphere_center.get_vect_x();
		double sphere_center_y = sphere_center.get_vect_y();
		double sphere_center_z = sphere_center.get_vect_z();

		double b = (2 * (ray_origin_x - sphere_center_x) * ray_direction_x) +
			(2 * (ray_origin_y - sphere_center_y) * ray_direction_y) +
			(2 * (ray_origin_z - sphere_center_z) * ray_direction_z);
		double c = (ray_origin_x - sphere_center_x) * (ray_origin_x - sphere_center_x) +
			(ray_origin_y - sphere_center_y) * (ray_origin_y - sphere_center_y) +
			(ray_origin_z - sphere_center_z) * (ray_origin_z - sphere_center_z) - (radius_ * radius_);

		const double discriminant = b * b - 4 * c;

		if (discriminant > 0) {
			//the ray intersects the sphere (at 2 positions)
			const double root_1 = ((-1 * b - sqrt(discriminant)) / 2) - 0.000001;

			if (root_1 > 0) {
				// the first route is the smallest positive root_
				return root_1;
			}
			else {
				// the second root is the smallest root
				const double root_2 = ((sqrt(discriminant) - b) / 2) - 0.000001;
				return root_2;
			}
		}
		else {
			// the ray missed the Sphere
			return -1;
		}
	}
};

inline sphere::sphere() {
	center_ = my_vector(0, 0, 0);
	radius_ = 1.0;
	color_ = color(.5, .5, .5, 0);
}

inline sphere::sphere(my_vector centerValue, double radiusValue, color colorValue) {
	center_ = centerValue;
	radius_ = radiusValue;
	color_ = colorValue;
}

#endif
