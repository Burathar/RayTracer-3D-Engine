#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "Object.h"
#include "MyVector.h"
#include "Color.h"

class triangle : public object {
	my_vector a_, b_, c_;
	my_vector normal_;
	double distance_;
	color color_;

public:

	triangle();

	triangle(my_vector, my_vector, my_vector, color);

	my_vector get_triangle_normal() {
		my_vector ca(c_.get_vect_x() - a_.get_vect_x(), c_.get_vect_y() - a_.get_vect_y(), c_.get_vect_z() - a_.get_vect_z());
		const my_vector ba(b_.get_vect_x() - a_.get_vect_x(), b_.get_vect_y() - a_.get_vect_y(), b_.get_vect_z() - a_.get_vect_z());
		normal_ = ca.cross_product(ba).normalize();
		return normal_;
	}

	double get_triangle_distance() {
		normal_ = get_triangle_normal();
		distance_ = normal_.dot_product(a_);
		return distance_;
	}

	color get_color() override { return color_; }

	my_vector get_normal_at(my_vector point) override
	{
		normal_ = get_triangle_normal();

		return normal_;
	}

	double find_intersection(ray ray) override
	{
		my_vector ray_direction = ray.get_ray_direction();
		my_vector ray_origin = ray.get_ray_origin();

		normal_ = get_triangle_normal();
		distance_ = get_triangle_distance();

		const double a = ray_direction.dot_product(normal_);

		if (a == 0) {
			//ray is parallel to  the Triangle
			return -1;
		}
		const double b = normal_.dot_product(ray.get_ray_origin().vect_add(normal_.vect_mult(distance_).negative()));
		const double distance_to_plane = -1 * b / a;

		//Is the intersection in the triangle?
		const double qx = ray_direction.vect_mult(distance_to_plane).get_vect_x() + ray_origin.get_vect_x();
		const double qy = ray_direction.vect_mult(distance_to_plane).get_vect_y() + ray_origin.get_vect_y();
		const double qz = ray_direction.vect_mult(distance_to_plane).get_vect_z() + ray_origin.get_vect_z();

		my_vector q(qx, qy, qz);

		my_vector ca(c_.get_vect_x() - a_.get_vect_x(), c_.get_vect_y() - a_.get_vect_y(), c_.get_vect_z() - a_.get_vect_z());
		const my_vector qa(q.get_vect_x() - a_.get_vect_x(), q.get_vect_y() - a_.get_vect_y(), q.get_vect_z() - a_.get_vect_z());
		const double test1 = (ca.cross_product(qa)).dot_product(normal_);

		my_vector bc(b_.get_vect_x() - c_.get_vect_x(), b_.get_vect_y() - c_.get_vect_y(), b_.get_vect_z() - c_.get_vect_z());
		const my_vector qc(q.get_vect_x() - c_.get_vect_x(), q.get_vect_y() - c_.get_vect_y(), q.get_vect_z() - c_.get_vect_z());
		const double test2 = (bc.cross_product(qc)).dot_product(normal_);

		my_vector ab(a_.get_vect_x() - b_.get_vect_x(), a_.get_vect_y() - b_.get_vect_y(), a_.get_vect_z() - b_.get_vect_z());
		const my_vector qb(q.get_vect_x() - b_.get_vect_x(), q.get_vect_y() - b_.get_vect_y(), q.get_vect_z() - b_.get_vect_z());
		const double test3 = (ab.cross_product(qb)).dot_product(normal_);

		if ((test1 >= 0) && (test2 >= 0) && (test3 >= 0)) {
			//inside the triangle
			return -1 * b / a;
		}
		return -1;
	}
};

inline triangle::triangle() {
	a_ = my_vector(1, 0, 0);
	b_ = my_vector(0, 1, 0);
	c_ = my_vector(0, 0, 1);
	normal_ = my_vector(1, 0, 0);
	distance_ = 0.0;
	color_ = color(.5, .5, .5, 0);
}

inline triangle::triangle(my_vector point_a, my_vector point_b, my_vector point_c, color color_value) {
	a_ = point_a;
	b_ = point_b;
	c_ = point_c;
	color_ = color_value;
	distance_ = 0.0;
}

#endif
